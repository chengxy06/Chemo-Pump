/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-10 jlli
* Initial revision.
*
************************************************/
#include "app_delivery.h"
#include "infusion_motor.h"
#include "three_valve_motor.h"
#include "drv_stf_motor.h"
#include "mid_rtc.h"
#include "msg.h"
#include "ssz_common.h"
#include "data.h"
#include "alarm.h"
#include "ssz_time_utility.h"
#include "common.h"
#include "mid_common.h"
#include "param.h"
#include "app_common.h"
#include "param_internal.h"
#include "pressure_bubble_sensor.h"
#include "app_mcu_monitor.h"

/************************************************
* Declaration
************************************************/
#define DELIVERY_POINT_CHECK_TIME_MS 20

void app_delivery_run_internal(InfusionType infusion_type);
void app_delivery_pause_internal(InfusionType infusion_type, int cause);
void app_delivery_infuse_one_point_dose(InfusionType infusion_type, int dose, InfusionBagMeds infusion_bag, int infusion_speed);
void app_delivery_set_next_infuse_point(InfusionType infusion_type);

/************************************************
* Variable 
************************************************/
InfusionTime g_next_delivery_time;
int  g_next_delivery_dose = 0;
InfusionBagMeds g_next_delivery_bag;
int g_next_delivery_speed;

InfusionTime g_last_delivery_time;

int32_t g_infuse_start_time;
InfusionType g_curr_infusion_type;
InfusionBagMeds g_curr_delivery_bag;
int g_curr_delivery_except_encoder;
int g_curr_delivery_speed;
bool g_is_infusing_point;

InfusionInfo g_infusion_data_info[kInfusionTypeMax];
//每次多转的编码数
static int g_infusion_deviation_encoder = 0;

static int g_infused_encoder_intpart = 0;
static float g_infused_encoder_decpart = 0.0;

/************************************************
* Function 
************************************************/
//return the diff ms
int32_t app_delivery_get_diff_ms_from_now(const InfusionTime* dest_time)
{
	if (dest_time->is_use_rtc) {
		if (dest_time->rtc_seconds == -1) {
			return INT32_MAX;
		} else {
			int32_t current_time = ssz_time_now_seconds();
			int tmp = dest_time->rtc_seconds - current_time;
			return tmp * 1000 + dest_time->rtc_millisecond;
		}
	} else {
		return ssz_tick_time_diff(ssz_tick_time_now(), dest_time->tick_time);
	}
}

void app_delivery_infuse_one_point_dose_after_ms() 
{
    app_delivery_infuse_one_point_dose(g_curr_infusion_type, g_next_delivery_dose, g_next_delivery_bag, g_next_delivery_speed);
}

void app_delivery_check_delivery_point()
{
	if (!g_next_delivery_time.is_use_rtc) {
		//if not use RTC, no need check
		return;
	}
	int32_t diff_ms;

	diff_ms = app_delivery_get_diff_ms_from_now(&g_next_delivery_time);

	if (diff_ms < 0) {
		//if pass the time, set next delivery time point
		app_delivery_set_next_infuse_point(g_curr_infusion_type);
		diff_ms = app_delivery_get_diff_ms_from_now(&g_next_delivery_time);
	}
	
	//check the diff
	if (diff_ms == 0) {
		//has reached delivery time
		app_delivery_infuse_one_point_dose(g_curr_infusion_type, g_next_delivery_dose, g_next_delivery_bag, g_next_delivery_speed);
	} else if (diff_ms < 1000) { //if less than 1 second, need use timer
	
		timer_stop(kTimerInfusionPointCheck);
		timer_start_oneshot_after(kTimerStartInfusionAfterMs, diff_ms);
	}

}
int app_delivery_get_a_bag_infusion_speed(InfusionType infusion_type, const InfusionTime* infusion_time)
{
	const InfusionInfo* info = &g_infusion_data_info[infusion_type];
	int infusion_speed = info->infusion_speed;

	//if peak mode is enabled, set peak mode dose according to next delivery time
	if (info->is_peek_infusion_mode) {
		SszDateTime time_tmp;
		if (infusion_time && infusion_time->is_use_rtc) {
			ssz_seconds_to_time(infusion_time->rtc_seconds, &time_tmp);
		} else {
			time_tmp = *ssz_time_now();
		}
        
		if (info->peek_infusion_start_time_point <= info->peek_infusion_end_time_point) {
			if (time_tmp.hour >= info->peek_infusion_start_time_point && time_tmp.hour < info->peek_infusion_end_time_point) {
				infusion_speed = info->peek_infusion_speed;
			}
		} else if (info->peek_infusion_start_time_point > info->peek_infusion_end_time_point) {
			if (time_tmp.hour < info->peek_infusion_end_time_point) {
				infusion_speed = info->peek_infusion_speed;
			} else if (time_tmp.hour >= info->peek_infusion_start_time_point) {
				infusion_speed = info->peek_infusion_speed;
			}
		}
	}

	return infusion_speed;
}
void app_delivery_get_a_bag_next_infuse_dose(InfusionType infusion_type,
        const InfusionTime* next_infusion_time, InfusionBagMeds* infuse_bag, int *infuse_dose)
{
	const InfusionInfo* info = &g_infusion_data_info[infusion_type];
	int infusion_speed = app_delivery_get_a_bag_infusion_speed(infusion_type, next_infusion_time);

	if (info->total_dose == -1) {
		//no limit total dose, run never stop
		if (infusion_type == kNormalInfusion) {
            *infuse_dose = infusion_speed / 12;
		} else {
			*infuse_dose = -1;
		}
		*infuse_bag = kBagAMeds;
	} else if (info->infused_dose < info->total_dose) {
		if (infusion_type == kNormalInfusion) {
			//use user set speed
			*infuse_dose = infusion_speed / 12;
		} else {
			*infuse_dose = info->total_dose -
				info->infused_dose;
		}
		//if it overrun, fix it
		if (*infuse_dose > info->total_dose - info->infused_dose) {
			*infuse_dose = info->total_dose - info->infused_dose;
		}
		*infuse_bag = kBagAMeds;
	} else {
		//infusion is complete
		*infuse_dose = 0;
		*infuse_bag = kBagBull;
	}
}
void app_delivery_get_b_bag_next_infuse_dose(InfusionType infusion_type, InfusionBagMeds* infuse_bag, int *infuse_dose)
{
	const InfusionInfo* info = &g_infusion_data_info[infusion_type];
	if (info->meds_b_total_dose == -1) {
		//no limit B total dose, run never stop
		*infuse_dose = -1;
		*infuse_bag = kBagBMeds;
	} else if (info->meds_b_infused_dose < info->meds_b_total_dose+info->meds_b_additional_dose) {
		//use max speed to infusion B bag
		*infuse_dose = info->meds_b_total_dose+ info->meds_b_additional_dose - info->meds_b_infused_dose;
		*infuse_bag = kBagBMeds;
	} else {
		//infusion is complete
		*infuse_dose = 0;
		*infuse_bag = kBagBull;
	}
}
void app_delivery_get_next_infuse_dose(InfusionType infusion_type,
        const InfusionTime* next_infusion_time, InfusionBagMeds* infuse_bag, 
        int *infuse_dose)
{
	if (infusion_type==kNormalInfusion) {
		app_delivery_get_a_bag_next_infuse_dose(infusion_type, next_infusion_time, infuse_bag, infuse_dose);
		if (*infuse_bag == kBagBull) {
			app_delivery_get_b_bag_next_infuse_dose(infusion_type, infuse_bag, infuse_dose);
		}
	} else if (infusion_type == kCleanTube) {
		app_delivery_get_b_bag_next_infuse_dose(infusion_type, infuse_bag, infuse_dose);
	} else if (infusion_type == kClearAir) {
		app_delivery_get_b_bag_next_infuse_dose(infusion_type, infuse_bag, infuse_dose);
		if (*infuse_bag == kBagBull) {
			app_delivery_get_a_bag_next_infuse_dose(infusion_type, next_infusion_time, infuse_bag, infuse_dose);
		}
	} else {
		ssz_assert_fail();
	}
}
void app_delivery_get_next_infuse_time(InfusionType infusion_type,
        const InfusionTime* last_infusion_time, InfusionBagMeds* infuse_bag,
	    InfusionTime* infuse_time, int *interval_ms, int* infuse_speed)
{
	//const InfusionInfo* info = &g_infusion_data_info[infusion_type];
	int32_t curr_time;
	curr_time = ssz_time_now_seconds();

	if (*infuse_bag == kBagBull) {
		//not have delivery point
		infuse_time->is_use_rtc = true;
		infuse_time->rtc_seconds = -1;
		infuse_time->rtc_millisecond = 0;
		if (interval_ms)	*interval_ms = 0;
		if(infuse_speed) *infuse_speed = 0;
	} else if (*infuse_bag == kBagBMeds) {
		infuse_time->is_use_rtc = true;
		infuse_time->rtc_seconds = curr_time;//if delivery bag B, run at once
		infuse_time->rtc_millisecond = 0;
		if (interval_ms) *interval_ms = 0;
		if(infuse_speed) *infuse_speed = BAG_B_MEDS_INFUSION_SPEED;
	} else {
		//delivery bag A
		if (infusion_type == kClearAir) {
			infuse_time->is_use_rtc = true;
			infuse_time->rtc_seconds = curr_time;//if clear air, run at once
			infuse_time->rtc_millisecond = 0;
			if (interval_ms)*interval_ms = 0;
			if(infuse_speed) *infuse_speed = BAG_B_MEDS_INFUSION_SPEED;
		} else {
			int curr_infusion_speed = app_delivery_get_a_bag_infusion_speed(infusion_type, NULL);
			if(infuse_speed) *infuse_speed = curr_infusion_speed;

			int infusion_interval_secs;
			int infusion_interval_ms;
            //int infusion_interval_total_ms;
			if (curr_infusion_speed>0) {
				infusion_interval_secs = (int)(3600 / (dose_to_encoder(curr_infusion_speed) / SPEED_ENCODER_TO_INFUSION_ENCODER_REDUCTION_RATIO)); //delivery each 5 min  5 * 60
				infusion_interval_ms = ((int)((3600 / (dose_to_encoder(curr_infusion_speed) / SPEED_ENCODER_TO_INFUSION_ENCODER_REDUCTION_RATIO)) * 1000)) % 1000;
			} else {
				infusion_interval_secs = 5;
				infusion_interval_ms = 0;
			}

			if (interval_ms) *interval_ms = infusion_interval_secs * 1000 + infusion_interval_ms;
#ifdef SSZ_TARGET_SIMULATOR
			//infusion_interval_secs = 5;//for test, change it as 5 sec
#endif
//            infusion_interval_total_ms = infusion_interval_secs * 1000 + infusion_interval_ms;
//            if(infusion_interval_total_ms - (600 - 110)>= 0){
//                infusion_interval_total_ms -= (600 - 110);
//                infusion_interval_secs = infusion_interval_total_ms / 1000;
//                infusion_interval_ms = infusion_interval_total_ms % 1000;
//            }

			if (infusion_interval_secs > 0) {
				//if the interval is big than one second, use RTC
				infuse_time->is_use_rtc = true;
				//if have last delivery time, calc base on the last delivery time 
				if (last_infusion_time && last_infusion_time->is_use_rtc) {
					infuse_time->rtc_seconds = last_infusion_time->rtc_seconds + infusion_interval_secs;
					infuse_time->rtc_millisecond = last_infusion_time->rtc_millisecond + infusion_interval_ms;

					if (infuse_time->rtc_millisecond >= 1000) {
						infuse_time->rtc_millisecond -= 1000;
						infuse_time->rtc_seconds++;
					}

					//if the time is old than now, add interval until later than now
					while (infuse_time->rtc_seconds < curr_time) {
						infuse_time->rtc_seconds += infusion_interval_secs;
						infuse_time->rtc_millisecond += infusion_interval_ms;
						if (infuse_time->rtc_millisecond >= 1000) {
							infuse_time->rtc_millisecond -= 1000;
							infuse_time->rtc_seconds++;
						}
					}

				} else {
					infuse_time->rtc_seconds = curr_time + infusion_interval_secs;
					infuse_time->rtc_millisecond = infusion_interval_ms;
				}

			} else {
				//small than one second, use systick
				infuse_time->is_use_rtc = false;
				//if have last delivery time, calc base on the last delivery time 
				if (last_infusion_time && last_infusion_time->is_use_rtc == false) {
					infuse_time->tick_time = ssz_tick_time_next_ex(last_infusion_time->tick_time,
						infusion_interval_ms);
					if (!ssz_tick_time_is_after_or_eq(infuse_time->tick_time, ssz_tick_time_now())) {
						//if the time is old than now, set as now
						infuse_time->tick_time = ssz_tick_time_now();
					}
				} else {
					infuse_time->tick_time = ssz_tick_time_next(infusion_interval_ms);
				}
			}
		}
	}
}
//get next infuse point's type, dose and time, delivery interval, infuse speed
//return next delivery point's diff milliseconds from now time,  -1: no infuse point
int app_delivery_get_next_infuse_point(InfusionType infusion_type, const InfusionTime* last_infusion_time, InfusionBagMeds* infuse_bag, int *infuse_dose,
	InfusionTime* infuse_time, int *interval_ms, int* infuse_speed)
{
	app_delivery_get_next_infuse_dose(infusion_type, last_infusion_time, infuse_bag, infuse_dose);
	int32_t curr_time = ssz_time_now_seconds();
	app_delivery_get_next_infuse_time(infusion_type, last_infusion_time, infuse_bag, infuse_time, interval_ms, infuse_speed);

	//use new next infuse time to recalc next infuse dose
	app_delivery_get_next_infuse_dose(infusion_type, infuse_time, infuse_bag, infuse_dose);
	
	if (infuse_time->is_use_rtc && infuse_time->rtc_seconds == -1) {
		return -1;
	} else if (infuse_time->is_use_rtc) {
		return (infuse_time->rtc_seconds - curr_time)*1000 + infuse_time->rtc_millisecond;
	} else {
		return ssz_tick_time_elapsed_ex(ssz_tick_time_now(), infuse_time->tick_time);
	}
}
//set next infuse type, dose and time point
void app_delivery_set_next_infuse_point(InfusionType infusion_type)
{
	int ret = app_delivery_get_next_infuse_point(infusion_type, &g_last_delivery_time, 
		&g_next_delivery_bag, &g_next_delivery_dose, 
		&g_next_delivery_time, NULL, &g_next_delivery_speed);
	if (ret != -1) {
		char buff[20];
		if (g_next_delivery_time.is_use_rtc) {
			SszDateTime tmp;
			ssz_seconds_to_time(g_next_delivery_time.rtc_seconds, &tmp);
			common_printfln("set next infusion time: %s.%d", ssz_time_str(&tmp, buff), g_next_delivery_time.rtc_millisecond);
		} else {
			common_printfln("set next infusion tick time: %s", ssz_tick_time_str(g_next_delivery_time.tick_time, buff, sizeof(buff)));
		}

		common_printfln("set next infusion dose:%duL, bag: %d, speed:%duL/hr",
			g_next_delivery_dose, g_next_delivery_bag, g_next_delivery_speed);
	} else {
		common_printfln("no next infusion");
	}
}

void app_delivery_update_infusion_data_info(InfusionType infusion_type, InfusionBagMeds infusion_bag, int real_run_encoder)
{
	InfusionInfo* info = &g_infusion_data_info[infusion_type];
    float dose;

    if (infusion_bag == kBagAMeds) {
        dose =  encoder_to_dose_float(real_run_encoder);
        g_infused_encoder_intpart  = (int)dose;
        g_infused_encoder_decpart += dose - g_infused_encoder_intpart;

        if(g_infused_encoder_decpart >= 1.0){
            g_infused_encoder_decpart -= 1.0;
            g_infused_encoder_intpart +=1;
        }

        info->infused_dose += g_infused_encoder_intpart;

    } else if(infusion_bag == kBagBMeds) {
        info->meds_b_infused_dose += encoder_to_dose(real_run_encoder);
    }

	if (infusion_type == kNormalInfusion) {
		data_write(kDataInfusionInfoBlock, info, sizeof(*info));
	}
}

void app_delivery_on_motor_stop(int run_encoder, int stop_cause)
{
	if (g_is_infusing_point) {
		g_is_infusing_point = false;
		msg_post_int(kMsgInfuseOnePointStop, g_curr_infusion_type);
		if (g_curr_delivery_bag == kBagAMeds && g_curr_infusion_type == kNormalInfusion &&
			stop_cause == MOTOR_STOP_BY_FINISH_EXPECT_TARGET) {
			//g_infusion_deviation_encoder = run_encoder - g_curr_delivery_except_encoder;
			//because we let speed encoder to stop, so not care the infusion encoder deviation
			g_infusion_deviation_encoder = 0;
		} else {
			g_infusion_deviation_encoder = 0;
		}

        app_delivery_update_infusion_data_info(g_curr_infusion_type, g_curr_delivery_bag, run_encoder);
        common_printfln("[%s]--one point infusion stop, run dose[uL]--int part:%d, decpart:%f",
			ssz_time_now_str(), g_infused_encoder_intpart, g_infused_encoder_decpart);

		if (app_delivery_state(g_curr_infusion_type) == kInfusionRunning) {
			if (stop_cause == MOTOR_STOP_BY_FINISH_EXPECT_TARGET) {
				//stop because finish target, check if finish
				if (app_delivery_is_finish_expect_target(g_curr_infusion_type)) {
					//pause infusion for finish
					app_delivery_pause_internal(g_curr_infusion_type, DELIVERY_PAUSE_BY_FINISH_DELIVERY);
				} else {
					//set next infusion point
					//app_delivery_set_next_infuse_point(g_curr_infusion_type);
					int32_t next_infusion_diff_ms = app_delivery_get_diff_ms_from_now(&g_next_delivery_time);

					if (next_infusion_diff_ms <= 0) {
						//need run at once, use msg can avoid 
						//app_delivery_infuse_one_point_dose->app_delivery_on_motor_stop->app_delivery_infuse_one_point_dose
						//->app_delivery_on_motor_stop...
						msg_post_int(kMsgInfuseNextOnePointAtOnce, 0);
					} else if (g_next_delivery_time.is_use_rtc) {
						//use period timer to check 
						timer_start_periodic_every(kTimerInfusionPointCheck, DELIVERY_POINT_CHECK_TIME_MS);
					} else {
						//use systick
						timer_start_oneshot_at(kTimerStartInfusionAfterMs, g_next_delivery_time.tick_time);
					}
				}
			}else {
				//stop by unknow cause, need pause
				app_delivery_pause_internal(g_curr_infusion_type, DELIVERY_PAUSE_BY_OTHER);
			}
		}

	}
}
void app_delivery_infuse_one_point_dose(InfusionType infusion_type,
                    int dose, InfusionBagMeds infusion_bag, int infusion_speed)
{
	bool is_three_valve_right = true;

#ifdef SSZ_TARGET_MACHINE
    if (drv_three_valve_is_pos_a_detected() && drv_three_valve_is_pos_b_detected()) {
        alarm_set(kAlarmThreeValveMotorStuckID);
    }
#endif 
	if (infusion_bag == kBagAMeds) {
		//set valve to right position before infusion starts
		if (!drv_three_valve_is_pos_a_detected()) {
			is_three_valve_right = three_valve_motor_move_to_position_a();
		}
	} else if (infusion_bag == kBagBMeds) {
		if (!drv_three_valve_is_pos_b_detected()) {
			is_three_valve_right = three_valve_motor_move_to_position_b();
		}
	}

	if (!is_three_valve_right) {
		app_delivery_pause(infusion_type);
		return;
	}

	int expect_encoder;
	int real_expect_encoder;
	if (dose == -1) {
		expect_encoder = -1;
	} else {
		expect_encoder = dose_to_encoder(dose);
	}
	real_expect_encoder = expect_encoder;
	if (real_expect_encoder != -1) {
		if (infusion_bag == kBagAMeds&&infusion_type == kNormalInfusion) {
			if (g_infusion_deviation_encoder <= expect_encoder) {
				real_expect_encoder = expect_encoder - g_infusion_deviation_encoder;
				g_infusion_deviation_encoder = 0;
			} else {
				real_expect_encoder = 0;
				g_infusion_deviation_encoder -= expect_encoder;
			}
		}
	}

	common_printfln("[%s]--one point infusion start, bag[%d], speed[%duL/hr]",
		ssz_time_now_str(), infusion_bag, infusion_speed);

	g_is_infusing_point = true;
	g_curr_infusion_type = infusion_type;
	g_curr_delivery_bag = infusion_bag;
	g_curr_delivery_speed = infusion_speed;
	g_curr_delivery_except_encoder = real_expect_encoder;
	g_last_delivery_time = g_next_delivery_time;
	timer_stop(kTimerInfusionPointCheck);
	timer_stop(kTimerStartInfusionAfterMs);

    app_delivery_set_next_infuse_point(g_curr_infusion_type);

    if (real_expect_encoder == 0) {
		app_delivery_on_motor_stop(0, MOTOR_STOP_BY_FINISH_EXPECT_TARGET);
	} else {
		bool is_start_success;
		msg_post_int(kMsgInfuseOnePointStart, infusion_type);
		if (infusion_bag == kBagAMeds&&infusion_type == kNormalInfusion) {
			is_start_success = infusion_motor_start_ex(kForward, -1, app_delivery_on_motor_stop);//real_expect_encoder
		} else {
			is_start_success = infusion_motor_start(kForward, real_expect_encoder, app_delivery_on_motor_stop);//real_expect_encoder
		}

		if (!is_start_success) {
			app_delivery_on_motor_stop(0, MOTOR_STOP_CUSTOM_CAUSE_START);
		}
	}

}

void app_delivery_on_infuse_next_one_point_at_once(MsgParam param)
{
	app_delivery_infuse_next_one_point_at_once();
}
void app_delivery_init()
{
	ssz_mem_zero(&g_infusion_data_info, sizeof(g_infusion_data_info));
	msg_set_handler(kMsgInfuseNextOnePointAtOnce, app_delivery_on_infuse_next_one_point_at_once);
}
//set the state as ready
void app_delivery_set_ready(InfusionType infusion_type)
{
	g_infusion_data_info[infusion_type].infusion_state = kInfusionReady;
}
//all variable's unit is 0.001ml
//param.total_dose: -1->will never auto stop when delivery A bag
//param.meds_b_total_dose: -1->will never auto stop when delivery B bag
void app_delivery_set_infusion_param(InfusionType infusion_type,
            InfusionParam* param, int infused_dose, int meds_b_infused_dose)
{
	ssz_mem_zero(&g_infusion_data_info[infusion_type], sizeof(g_infusion_data_info[infusion_type]));
	g_infusion_data_info[infusion_type].total_dose = param->total_dose;
	g_infusion_data_info[infusion_type].infused_dose = infused_dose;
	//g_infusion_data_info[infusion_type].first_infusion_dose = param->first_infusion_dose;
	g_infusion_data_info[infusion_type].meds_b_total_dose = param->meds_b_total_dose;
	g_infusion_data_info[infusion_type].meds_b_infused_dose = meds_b_infused_dose;
	g_infusion_data_info[infusion_type].infusion_speed = param->infusion_speed;
	g_infusion_data_info[infusion_type].is_peek_infusion_mode = param->is_peek_infusion_mode;
	g_infusion_data_info[infusion_type].peek_infusion_speed = param->peek_infusion_speed;
	g_infusion_data_info[infusion_type].peek_infusion_start_time_point= param->peek_infusion_start_time_point;
	g_infusion_data_info[infusion_type].peek_infusion_end_time_point = param->peek_infusion_end_time_point;

    if ( (param->meds_b_total_dose>0 &&infusion_type==kNormalInfusion) || 
		infusion_type == kCleanTube ) {
		g_infusion_data_info[infusion_type].meds_b_additional_dose = INFUSION_TUBE_VOLUME_UL;
	}
	//g_infusion_data_info[infusion_type].infusion_type = infusion_type;
	app_delivery_set_ready(infusion_type);
}
bool app_delivery_is_can_run()
{
	AlarmLevel highest_level = alarm_highest_priority_level();

	if (highest_level >= kAlarmLevelHigh) {
		return false;
	}

	return true;
}
bool app_delivery_start(InfusionType infusion_type)
{
	if (!app_delivery_is_can_run()) {
		return false;
	}

	common_printfln("[%s]--infusion start, total dose[%duL], meds_b_total_dose[%duL],"
		"speed[%duL/hr]",
		ssz_time_now_str(), g_infusion_data_info[infusion_type].total_dose,
		g_infusion_data_info[infusion_type].meds_b_total_dose,
		g_infusion_data_info[infusion_type].infusion_speed);
	msg_post_two_param(kMsgInfusionStart, infusion_type, DELIVERY_START_TO_RUN);
	app_delivery_run_internal(infusion_type);
	return true;
}
//start to pause
bool app_delivery_start_to_pause(InfusionType infusion_type)
{
	g_infusion_data_info[infusion_type].infusion_state = kInfusionPause;
	timer_stop(kTimerInfusionPointCheck);
	timer_stop(kTimerStartInfusionAfterMs);
	msg_post_two_param(kMsgInfusionStart, infusion_type, DELIVERY_START_TO_PAUSE);

	common_printfln("[%s]--infusion start to pause, total dose[%duL], meds_b_total_dose[%duL],"
		"speed[%duL/hr]",
		ssz_time_now_str(), g_infusion_data_info[infusion_type].total_dose,
		g_infusion_data_info[infusion_type].meds_b_total_dose,
		g_infusion_data_info[infusion_type].infusion_speed);

	return true;
}
//start infuse
bool app_delivery_start_with_param(InfusionType infusion_type, InfusionParam* param)
{
	app_delivery_set_infusion_param(infusion_type, param, 0, 0);
	return app_delivery_start(infusion_type);
}

void app_delivery_run_internal(InfusionType infusion_type)
{
	if(!is_pressure_adc_ready_after_installed())
	{
		pressure_and_bubble_sensor_pwr_enable();
		
		//set watchodog time as 6000ms, because it will delay long time
		watchdog_set_expired_time(6000);
		
		ssz_delay_ms(1000);
		prepare_pressure_adc_before_infusion_start();

		//restore watchdog time
		watchdog_set_expired_time(WATCHDOG_EXPIRED_TIME_MS);
	}
	g_infusion_data_info[infusion_type].infusion_state = kInfusionRunning;
	g_infuse_start_time = ssz_time_now_seconds();
	g_infusion_deviation_encoder = 0;
    g_infused_encoder_decpart = 0.0;

	timer_set_handler(kTimerInfusionPointCheck, app_delivery_check_delivery_point);
	timer_set_handler(kTimerStartInfusionAfterMs, app_delivery_infuse_one_point_dose_after_ms);

	InfusionTime delivery_time;
	int dose;
	InfusionBagMeds infusion_bag;
	int speed;
	app_delivery_get_next_infuse_point(infusion_type, NULL, &infusion_bag, &dose, &delivery_time, NULL, &speed);

	if (infusion_bag == kBagBull) {
		//if next no need infuse, it is complete
		app_delivery_pause_internal(infusion_type, DELIVERY_PAUSE_BY_FINISH_DELIVERY);
	} else {
		g_next_delivery_time.rtc_seconds = ssz_time_now_seconds();
		g_next_delivery_time.rtc_millisecond = 0;
		g_next_delivery_time.tick_time = ssz_tick_time_now();
		g_next_delivery_time.is_use_rtc = delivery_time.is_use_rtc;
		app_delivery_infuse_one_point_dose(infusion_type, dose, infusion_bag, speed);
	}
}
bool app_delivery_resume(InfusionType infusion_type) {
	if (!app_delivery_is_can_run()) {
		return false;
	}
    
	if (g_infusion_data_info[infusion_type].infusion_state == kInfusionPause) {
		common_printfln("[%s]--infusion run, total dose[%duL], speed[%duL/hr],"
			"infused dose[%duL],b_infused_dose[%duL]",
			ssz_time_now_str(), g_infusion_data_info[infusion_type].total_dose,
			g_infusion_data_info[infusion_type].infusion_speed,
			g_infusion_data_info[infusion_type].infused_dose, g_infusion_data_info[infusion_type].meds_b_infused_dose);
		msg_post_two_param(kMsgInfusionResume, infusion_type, 0);
		app_delivery_run_internal(infusion_type);
		return true;
	} else {
		return false;
	}
}
void app_delivery_pause_internal(InfusionType infusion_type, int cause)
{
	if (g_infusion_data_info[infusion_type].infusion_state == kInfusionRunning) {
		g_infusion_data_info[infusion_type].infusion_state = kInfusionPause;
		if(infusion_type == kClearAir){
			//when clear air finish, set the installed as false
			g_is_pressure_adc_ready_when_installed = false;
		}
		infusion_motor_stop(0);
		common_printfln("[%s]--infusion pause, infused dose[%duL],b_infused_dose[%duL]",
			ssz_time_now_str(), g_infusion_data_info[infusion_type].infused_dose,
			g_infusion_data_info[infusion_type].meds_b_infused_dose);
		timer_stop(kTimerInfusionPointCheck);
		timer_stop(kTimerStartInfusionAfterMs);
		msg_post_two_param(kMsgInfusionPause, infusion_type, cause);
	}
}

void app_delivery_pause(InfusionType infusion_type) {
	app_delivery_pause_internal(infusion_type, DELIVERY_PAUSE_BY_OTHER);
}

void app_delivery_stop(InfusionType infusion_type)
{
	if (g_infusion_data_info[infusion_type].infusion_state == kInfusionRunning ||
		g_infusion_data_info[infusion_type].infusion_state == kInfusionPause) {
		int16_t msg_param = DELIVERY_STOP_FROM_RUN; 
		
		if (g_infusion_data_info[infusion_type].infusion_state == kInfusionPause) {
			msg_param = DELIVERY_STOP_FROM_PAUSE;
		} else {
			if (infusion_type == kClearAir) {
				//when clear air finish, set the installed as false
				g_is_pressure_adc_ready_when_installed = false;
			}
		}
		g_infusion_data_info[infusion_type].infusion_state = kInfusionStop;
		infusion_motor_stop(0);

		common_printfln("[%s]--infusion stop, infused dose[%duL],b_infused_dose[%duL]",
			ssz_time_now_str(), g_infusion_data_info[infusion_type].infused_dose,
			g_infusion_data_info[infusion_type].meds_b_infused_dose);
		timer_stop(kTimerInfusionPointCheck);
		timer_stop(kTimerStartInfusionAfterMs);
		msg_post_two_param(kMsgInfusionStop, infusion_type, msg_param);
		
		if (infusion_type == kNormalInfusion) {
			InfusionInfo info;
			ssz_mem_zero(&info, sizeof(info));
			data_write(kDataInfusionInfoBlock, &info, sizeof(info));
		}

		//when infusion stop, change to B bag
		if (infusion_type != kClearAir) {
			three_valve_motor_move_to_position_b();
		}
	} else {
		g_infusion_data_info[infusion_type].infusion_state = kInfusionStop;
	}

}

InfusionState app_delivery_state(InfusionType infusion_type)
{
	return g_infusion_data_info[infusion_type].infusion_state;
}
InfusionBagMeds app_delivery_curr_bag(InfusionType infusion_type)
{
	if (app_delivery_state(infusion_type)==kInfusionRunning) {
		return g_curr_delivery_bag;
	}  else if (app_delivery_a_bag_rest_time_secs(infusion_type) != 0) {
		return kBagAMeds;
	} else {
		return kBagBMeds;
	}
}
int app_delivery_curr_speed(InfusionType infusion_type)
{
	if (app_delivery_state(infusion_type) == kInfusionRunning) {
		return g_curr_delivery_speed;
	} else if (app_delivery_a_bag_rest_time_secs(infusion_type) != 0) {
		return app_delivery_get_a_bag_infusion_speed(infusion_type, NULL);
	} else {
		return BAG_B_MEDS_INFUSION_SPEED;
	}
}
bool app_delivery_is_infusing_one_point(InfusionType infusion_type) {
	if (app_delivery_state(infusion_type) == kInfusionRunning && g_is_infusing_point) {
		return true;
	} else {
		return false;
	}
}
int app_delivery_infused_dose(InfusionType infusion_type)
{
	int curr_point_dose = 0;

    if (g_is_infusing_point&&g_curr_delivery_bag==kBagAMeds&&
		g_curr_infusion_type== infusion_type) {
		curr_point_dose = encoder_to_dose(infusion_motor_encoder_after_start());
	}
	return g_infusion_data_info[infusion_type].infused_dose + curr_point_dose;
}

int app_delivery_meds_b_infused_dose(InfusionType infusion_type)
{
	int curr_point_dose = 0;
	if (g_is_infusing_point && g_curr_delivery_bag == kBagBMeds &&
		g_curr_infusion_type == infusion_type) {
		curr_point_dose = encoder_to_dose(infusion_motor_encoder_after_start());
	}
	return g_infusion_data_info[infusion_type].meds_b_infused_dose + curr_point_dose;
}

int app_delivery_calc_a_bag_rest_time_secs(InfusionInfo *infusion_info)
{
    int rest_secs = 0;
    SszDateTime current_time;
    int current_time_seconds;

    if (infusion_info->total_dose == -1) {
		return -1;
	}
	
	int rest_a_dose = infusion_info->total_dose - infusion_info->infused_dose;


    if (infusion_info->is_peek_infusion_mode) {
        current_time = *ssz_time_now();
        current_time_seconds = current_time.hour * 3600 + current_time.minute * 60
                                + current_time.second;
        
        int peak_start = infusion_info->peek_infusion_start_time_point;
        int peak_end = infusion_info->peek_infusion_end_time_point;
        int peak_mode_expect_dose;
        
        int peak_mode_speed = infusion_info->peek_infusion_speed;
        int normal_speed = infusion_info->infusion_speed;
        
        
        int peak_mode_total_time = (peak_end > peak_start) ? (peak_end - peak_start) : (peak_end + 24- peak_start );
        peak_mode_expect_dose = peak_mode_total_time * peak_mode_speed;
        
        int peak_mode_remain_time_sec = (peak_end * 3600 - current_time_seconds) ? (peak_end *3600 - current_time_seconds) : (peak_end *3600 - current_time_seconds + 24 * 3600);
        int peak_mode_remain_dose = peak_mode_remain_time_sec / 3600.0 * peak_mode_speed;
        
        int before_peak_mode_time_sec = (peak_start * 3600 - current_time_seconds > 0) ? (peak_start *3600 - current_time_seconds) : (peak_start *3600 - current_time_seconds + 24 * 3600);
        int before_peak_mode_expect_dose = before_peak_mode_time_sec / 3600.0 * normal_speed;

        int peak_mode_day_total_dose = peak_mode_expect_dose + (24 - peak_mode_total_time) * normal_speed;

        
        if (rest_a_dose > peak_mode_day_total_dose) {
            rest_secs += (rest_a_dose / peak_mode_day_total_dose) * 24 * 3600;
            rest_a_dose %= peak_mode_day_total_dose;
        }
        
        if (current_time.hour < peak_start || current_time.hour > peak_end){  //not at peak mode
            if ( rest_a_dose < before_peak_mode_expect_dose){
                rest_secs += rest_a_dose * 3600.0 / normal_speed;
            } else if (rest_a_dose >= before_peak_mode_expect_dose && rest_a_dose < before_peak_mode_expect_dose + peak_mode_expect_dose) {
                rest_secs += before_peak_mode_time_sec + (rest_a_dose - before_peak_mode_expect_dose) * 3600.0 / peak_mode_speed;
            } else {
                rest_secs += peak_mode_total_time * 3600 + (rest_a_dose - peak_mode_expect_dose) * 3600.0 / normal_speed;
            }
        } else {
            if (rest_a_dose < peak_mode_remain_dose){
                rest_secs += rest_a_dose *3600.0 / peak_mode_speed;
            } else {
                rest_secs += peak_mode_remain_time_sec + (rest_a_dose - peak_mode_remain_dose) * 3600.0 / normal_speed;
            }
        }

    } else if (rest_a_dose > 0) {
    	rest_secs = rest_a_dose * 3600.0 / infusion_info->infusion_speed;
    }
    
	return rest_secs;

}

//return the rest seconds to complete delivery a bag
//-1: never can complete
int app_delivery_a_bag_rest_time_secs(InfusionType infusion_type)
{
	return app_delivery_calc_a_bag_rest_time_secs(&g_infusion_data_info[infusion_type]);
}

int app_delivery_b_bag_rest_time_secs(InfusionType infusion_type)
{
    int rest_secs=0;
    int rest_b_dose;

    if (g_infusion_data_info[infusion_type].meds_b_total_dose == -1) {
		return -1;
	}

	rest_b_dose = g_infusion_data_info[infusion_type].meds_b_total_dose + g_infusion_data_info[infusion_type].meds_b_additional_dose
                      - g_infusion_data_info[infusion_type].meds_b_infused_dose;
	if (rest_b_dose > 0) {
		rest_secs = rest_b_dose * 3600 / (FLUSH_SPEED * 1000.0);
	}
	return rest_secs;
}
//is the infusion arrive the target
bool app_delivery_is_finish_expect_target_ex(InfusionInfo *info)
{
	if (info->total_dose == -1 ||
		info->meds_b_total_dose == -1) {
		return false;
	}

    int dose = info->total_dose - info->infused_dose;
	if (dose <= 0) {
		dose = info->meds_b_total_dose + info->meds_b_additional_dose - info->meds_b_infused_dose;
	}
	if (dose <= 0) {
		return true;
	}
	return false;
}


//is the infusion arrive the target
bool app_delivery_is_finish_expect_target(InfusionType infusion_type) 
{
	return app_delivery_is_finish_expect_target_ex(&g_infusion_data_info[infusion_type]);
}

InfusionInfo* app_delivery_info(InfusionType infusion_type)
{
	return &g_infusion_data_info[infusion_type];
}

void app_delivery_save_infusion_info(InfusionType infusion_type)
{
	data_write(kDataInfusionInfoBlock, &g_infusion_data_info[infusion_type], sizeof(g_infusion_data_info[0]));
}


//check if exist some infusion type running
bool app_delivery_is_exit_running_infusion()
{
	int i;
	for (i = 0; i < kInfusionTypeMax; i++) {
		//check if running
		if (app_delivery_state((InfusionType)i) == kInfusionRunning) {
			return true;
		}
	}

	return false;
}
//return current running infusion type, if no run, return kInfusionTypeMax
InfusionType app_delivery_curr_running_infusion()
{
	int i;
	for (i = 0; i < kInfusionTypeMax; i++) {
		//check if running
		if (app_delivery_state((InfusionType)i) == kInfusionRunning) {
			return (InfusionType)i;
		}
	}

	return kInfusionTypeMax;
}

//return next delivery point time
// if use_rtc=1 and rtc_second=-1, it indicate no next infuse point
// Note: it need call when it not infusing point
InfusionTime app_delivery_next_infuse_point_time()
{
	InfusionTime infuse_time;
	ssz_mem_zero(&infuse_time, sizeof(infuse_time));
	infuse_time.is_use_rtc = true;
	infuse_time.rtc_seconds = -1;

	if (app_delivery_is_exit_running_infusion()) {
		ssz_assert(g_is_infusing_point==false);
		infuse_time = g_next_delivery_time;
	}

	return infuse_time;
}

//return the diff ms from now to next delivery point
// when use RTC, because RTC not have millisecond,
//	so the value will have deviation which < 1 second
//if return INT32_MAX, it indicate no next infuse point
// Note: it need call when it not infusing point
int32_t  app_delivery_next_infuse_point_diff_ms_from_now()
{
	if (app_delivery_is_exit_running_infusion()) {
		ssz_assert(g_is_infusing_point == false);
		return app_delivery_get_diff_ms_from_now(&g_next_delivery_time);
	}
	else {
		//if not run, return the max value
		return INT32_MAX;
	}
}

//not wait next delivery time, infuse the next point at once
void app_delivery_infuse_next_one_point_at_once()
{
	if (app_delivery_is_exit_running_infusion()) {
		app_delivery_infuse_one_point_dose(g_curr_infusion_type, g_next_delivery_dose, g_next_delivery_bag, g_next_delivery_speed);
	}
}

//delivery is in peak mode
bool app_delivery_is_in_peak_mode(void) 
{
    SszDateTime time_tmp;
    bool ret = false;
	
    if (app_delivery_info(kNormalInfusion)->is_peek_infusion_mode) {
        time_tmp = *ssz_time_now();
    
        if (app_delivery_info(kNormalInfusion)->peek_infusion_start_time_point <= app_delivery_info(kNormalInfusion)->peek_infusion_end_time_point) {
            if (time_tmp.hour >= app_delivery_info(kNormalInfusion)->peek_infusion_start_time_point 
                    && time_tmp.hour < app_delivery_info(kNormalInfusion)->peek_infusion_end_time_point) {
                ret = true;
            }
        } else if (app_delivery_info(kNormalInfusion)->peek_infusion_start_time_point > app_delivery_info(kNormalInfusion)->peek_infusion_end_time_point) {
            if (time_tmp.hour < app_delivery_info(kNormalInfusion)->peek_infusion_end_time_point) {
                ret = true;
            } else if (time_tmp.hour >= app_delivery_info(kNormalInfusion)->peek_infusion_start_time_point) {
                ret = true;
            }
        }
    }
    return ret;
}


