/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-21 jlli
* Initial revision.
*
************************************************/
#include "three_valve_motor.h"
#include "drv_stf_motor.h"
#include "msg.h"
#include "ssz_common.h"
#include "timer.h"
#include "event.h"
#include "alarm.h"
#include "mid_adc.h"
#include "record_log.h"
#include "app_common.h"
#include "app_mcu_monitor.h"
#include "sys_power.h"
#include "mid_common.h"

/************************************************
* Declaration
************************************************/
#define THREE_VALVE_MOTOR_RUN_CHECK_TIME_MS 20
#define THREE_VALVE_MOTOR_STUCK_TIME_MS 1000
#define THREE_VALVE_MOTOR_START_CHECK_CURRENT_TIME_MS 50
#define THREE_VALVE_MOTOR_CURRENT_MAX 1500

#define THREE_VALVE_MOTOR_SENSOR_TO_MEET_POS 1
#define BATTERY_LOW_CHECK_COUNT 3

/************************************************
* Variable 
************************************************/
int g_three_valve_motor_encoder;
int g_three_valve_motor_expect_encoder;
int g_three_valve_motor_stop_cause;
MotorStopHandler g_three_valve_motor_stop_handler;
MotorState g_three_valve_motor_state;
MotorDirection g_three_valve_motor_running_dir;

//for monitor motor is run right
int g_three_valve_motor_direction_error_count;
bool g_three_valve_motor_is_event_set;
int  g_three_valve_motor_direction_error_total;


int g_three_valve_motor_last_encoder;
int32_t g_three_valve_motor_check_last_encoder_time;
int32_t g_three_valve_motor_monitor_start_time;
int g_three_valve_motor_soft_run_pwm_level = 0;

int g_three_valve_motor_run_start_pwm_level = 100;
int g_three_valve_motor_pwm_step_num = 5 ;

bool is_battery_low = false;
int g_three_valve_battery_low_counter;
int g_three_valve_battery_low_encoder_error_counter;
int g_three_valve_battery_when_empty;

/************************************************
* Function 
************************************************/

//return: true->success, false->error
bool three_valve_motor_move_to_position(int is_to_a)
{
	bool ret = true;
	bool is_pos_detected = false;


    //drv_three_valve_pos_detect_pwr_enable();
	//ssz_delay_us(100);

    if (drv_three_valve_is_pos_a_detected() && drv_three_valve_is_pos_b_detected()) {
        alarm_set(kAlarmThreeValveMotorStuckID);
        return false;
    }
		
#if THREE_VALVE_MOTOR_SENSOR_TO_MEET_POS
	if ((is_to_a && !drv_three_valve_is_pos_a_detected()) ||
		(!is_to_a && !drv_three_valve_is_pos_b_detected()) ) {
#endif
		//set watchodog time as 6000ms, because it will delay long time
		watchdog_set_expired_time(6000);


		if (is_to_a) {
			record_log_add_with_data(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
				kLogEventChangeToBigBag, NULL, 0);
			three_valve_motor_start(kBackward, -1, NULL);
		}
		else {
			record_log_add_with_data(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
				kLogEventChangeToSmallBag, NULL, 0);
			three_valve_motor_start(kForward, -1, NULL);
		}

		int current_ad;
		int voltage_ad;
		int stop_cause = MOTOR_STOP_BY_FINISH_EXPECT_TARGET;
		int motor_curren_error_count = 0;
		bool is_enable_check_current = false;
		int32_t curr_time = ssz_tick_time_now();
		int32_t curr_time_for_pwm = ssz_tick_time_now();
		int32_t curr_time_for_battery = ssz_tick_time_now();
        int battery = sys_power_battery_voltage();

		while (1) {
#ifdef THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE
			//	pwm control
			if ((ssz_tick_time_elapsed(curr_time_for_pwm) >= THREE_VALVE_MOTOR_START_PWM_RUN_TIME_MS) &&
				(g_three_valve_motor_soft_run_pwm_level < THREE_VALVE_MOTOR_START_PWM_MAX_LEVEL)) {
				curr_time_for_pwm = ssz_tick_time_now();
				g_three_valve_motor_soft_run_pwm_level = g_three_valve_motor_soft_run_pwm_level + g_three_valve_motor_pwm_step_num;
				if (is_to_a) {
					drv_three_valve_motor_change_duty_cycle_to_a(g_three_valve_motor_soft_run_pwm_level);
				}
				else {
					drv_three_valve_motor_change_duty_cycle_to_b(g_three_valve_motor_soft_run_pwm_level);
				}
				if (g_three_valve_motor_soft_run_pwm_level >= THREE_VALVE_MOTOR_START_PWM_MAX_LEVEL) {
					if (is_to_a) {
						drv_three_valve_motor_move_backward_ex();
					}
					else {
						drv_three_valve_motor_move_forward_ex();
					}
					motor_printfln("change to full speed");
				}
			}
#endif			
#if THREE_VALVE_MOTOR_SENSOR_TO_MEET_POS
			if (!is_pos_detected) {
				if (is_to_a && drv_three_valve_is_pos_a_detected() == true) {
					is_pos_detected = true;
					motor_printfln("detect hall, have run [%d]", g_three_valve_motor_encoder);
					g_three_valve_motor_expect_encoder = g_three_valve_motor_encoder+ 80;
					record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_INFO,
						kLogEventThreeValveMotorDetectPos,
						is_to_a,
						g_three_valve_motor_encoder);
				}
				if (!is_to_a && drv_three_valve_is_pos_b_detected() == true) {
					is_pos_detected = true;
					motor_printfln("detect hall, have run [%d]", g_three_valve_motor_encoder);
					g_three_valve_motor_expect_encoder = g_three_valve_motor_encoder+ 90;
					record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_INFO,
						kLogEventThreeValveMotorDetectPos,
						is_to_a,
						g_three_valve_motor_encoder);
				}
			}
#endif
			if (g_three_valve_motor_expect_encoder != -1 &&
				g_three_valve_motor_encoder >= g_three_valve_motor_expect_encoder)
			{
				break;
			}

			if (event_is_set(kEventThreeValveMotorDirectionWrong)) {
				event_clear(kEventThreeValveMotorDirectionWrong);
				record_log_add_with_one_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
					kLogEventThreeValveMotorDirectionError,
					g_infusion_motor_encoder);
				alarm_set(kAlarmThreeValveMotorDirectionErrorID);
				stop_cause = MOTOR_STOP_BY_DIRECTION_WRONG;
				ret = false;
				break;
			}
			// if (ssz_tick_time_elapsed(curr_time) >= 200)
			// {
			// 	motor_printfln("three  run [%d]", g_three_valve_motor_encoder);
			// 	//drv_three_valve_motor_sleep_enable();
			// 	drv_three_valve_motor_pwr_disable();
			// 	drv_three_valve_motor_brake();
			// 	//drv_three_valve_motor_inertial_stop();
			// 	ssz_delay_ms(50);
			// 	motor_printfln("three run [%d]", g_three_valve_motor_encoder);
			// 	break;
			// }
			if (ssz_tick_time_elapsed(curr_time)>=4000) {
                if (is_battery_low) {               
                    three_valve_motor_stop(MOTOR_STOP_BY_BATTERY_LOW);
                    msg_post_two_param(kMsgBatteryStatusWhenMotorRun, kBatteryEmpty, g_three_valve_battery_when_empty);
                } else {
       				record_log_add_with_one_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
    					kLogEventThreeValveMotorNotFindPos,
    					g_three_valve_motor_encoder);
    				alarm_set(kAlarmThreeValveMotorNotFindPos);
    				ret =false;
    				stop_cause = MOTOR_STOP_CUSTOM_CAUSE_START+ kAlarmThreeValveMotorNotFindPos; 	                 
                }
                break;
			}
			if (!is_enable_check_current && ssz_tick_time_elapsed(curr_time) >= THREE_VALVE_MOTOR_START_CHECK_CURRENT_TIME_MS) {
				is_enable_check_current = true;
			}
			if (is_enable_check_current) {
				current_ad = mid_adc_get_average_ADC_channel_value(kSTFDJCurrentADC);
				voltage_ad = M_get_voltage_from_adc(current_ad);
				//printf("[%d]three-way valve current:%d\n", ssz_tick_time_elapsed(NULL), current_ad);
				motor_monitor_printfln("three valve motor current[%d]=%dmv", current_ad, voltage_ad);
				if (voltage_ad > THREE_VALVE_MOTOR_CURRENT_MAX) {
					motor_curren_error_count++;
					motor_printfln("three valve motor current[%d]%dmv too large!", current_ad, voltage_ad);
				}
				else {
					motor_curren_error_count = 0;
				}
				if (motor_curren_error_count >= MOTOR_ALLOW_MOTOR_CURRENT_ERROR_COUNT) {
					stop_cause = MOTOR_STOP_BY_CURRENT_TOO_LARGE;
					motor_printfln("three valve motor current[%d]%dmv too large, stop!", current_ad, voltage_ad);
					record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
						kLogEventThreeValveMotorCurrentTooLarge,
						voltage_ad,
						g_three_valve_motor_encoder);
					alarm_set(kAlarmThreeValveMotorCurrentTooLargeID);
					ret = false;
					break;
				}
			}
#ifdef SSZ_TARGET_SIMULATOR
			break;
#endif
            //check battey when three valve motor run
            if (ssz_tick_time_elapsed(curr_time_for_battery) >= THREE_VALVE_MOTOR_CHECK_BATTERY_MS){
                curr_time_for_battery = ssz_tick_time_now();
                battery = sys_power_battery_voltage();
                motor_printfln("V%d",battery);
                if (!is_use_extern_power()) {
                    if (battery < BATT_LEVEL_EMPTY_WHEN_RUN)
                        g_three_valve_battery_low_counter++;
                
                    if (g_three_valve_battery_low_counter >= BATTERY_LOW_CHECK_COUNT) 
                        is_battery_low = true;
                }
            }
		}
		three_valve_motor_stop(stop_cause);
		//drv_three_valve_pos_detect_pwr_disable();

		//restore watchdog time
		watchdog_set_expired_time(WATCHDOG_EXPIRED_TIME_MS);
		
#if THREE_VALVE_MOTOR_SENSOR_TO_MEET_POS		
	}
#endif		
	event_clear(kEventThreeValveMotorDirectionWrong);
	event_clear(kEventThreeValveMotorFinishTarget);

	//drv_three_valve_pos_detect_pwr_disable();

	return ret;
}

bool three_valve_motor_move_to_position_a()
{
	return three_valve_motor_move_to_position(1);
}

bool three_valve_motor_move_to_position_b()
{
	return three_valve_motor_move_to_position(0);
}

void three_valve_motor_run_check() {
	//if the encoder is not changed, we think it is stuck, need stop motor
	if (ssz_tick_time_elapsed(g_three_valve_motor_check_last_encoder_time) >= THREE_VALVE_MOTOR_STUCK_TIME_MS) {
		g_three_valve_motor_check_last_encoder_time = ssz_tick_time_now();
		if (g_three_valve_motor_last_encoder == g_three_valve_motor_encoder) {
            if (is_battery_low) {               
                three_valve_motor_stop(MOTOR_STOP_BY_BATTERY_LOW);
                msg_post_two_param(kMsgBatteryStatusWhenMotorRun, kBatteryEmpty, g_three_valve_battery_when_empty);
            } else {
                if (g_three_valve_battery_low_encoder_error_counter < BATTERY_LOW_CHECK_COUNT) {
                    g_three_valve_battery_low_encoder_error_counter ++;
                   
                } else {
                    g_three_valve_battery_low_encoder_error_counter = 0;
                    three_valve_motor_stop(MOTOR_STOP_BY_STUCK);
                    alarm_set(kAlarmThreeValveMotorStuckID);
                }
            }
		}
		else {
			g_three_valve_motor_last_encoder = g_three_valve_motor_encoder;
		}

	}
	if (ssz_tick_time_elapsed(g_three_valve_motor_monitor_start_time) >= THREE_VALVE_MOTOR_START_CHECK_CURRENT_TIME_MS) {
		int current_ad = mid_adc_get_average_ADC_channel_value(kSTFDJCurrentADC);
		//motor_printfln("three motor current[%d]", current_ad);
		motor_monitor_printfln("three valve motor current[%d]=%dmv", current_ad, M_get_voltage_from_adc(current_ad));
		if (M_get_voltage_from_adc(current_ad) > THREE_VALVE_MOTOR_CURRENT_MAX) {
			//motor_printfln("three motor current[%d] too large!", current_ad);
			motor_printfln("three valve motor current[%d]%dmv too large, stop!", current_ad, M_get_voltage_from_adc(current_ad));
			three_valve_motor_stop(MOTOR_STOP_BY_CURRENT_TOO_LARGE);
			//alarm_set(kAlarmThreeValveMotorCurrentTooLargeID);
		}
	}

    //check battey when three valve motor run
    int battery = sys_power_battery_voltage();
    if (!is_use_extern_power()) {
        if (battery < BATT_LEVEL_EMPTY_WHEN_RUN){
            g_three_valve_battery_when_empty = battery;
            g_three_valve_battery_low_counter++;
        }
    
        if (g_three_valve_battery_low_counter >= BATTERY_LOW_CHECK_COUNT) 
            is_battery_low = true;
    }

}

void three_valve_motor_monitor_start() {
    g_three_valve_battery_low_counter = 0;
    is_battery_low = false;
	g_three_valve_motor_last_encoder = 0;
	g_three_valve_motor_check_last_encoder_time = ssz_tick_time_now();
	g_three_valve_motor_monitor_start_time = ssz_tick_time_now();
	timer_start_periodic_every(kTimerThreeValveMotorCheck, THREE_VALVE_MOTOR_RUN_CHECK_TIME_MS);
}
void three_valve_motor_monitor_stop() {
	timer_stop(kTimerThreeValveMotorCheck);
}
//start three-valve motor for the expected encoder in the set direction
void three_valve_motor_start(MotorDirection dir, int expect_encoder ,MotorStopHandler stop_handler)
{
	if (g_three_valve_motor_state == kMotorRun) {
		//need pause encoder count temporary for conflict
	}
	else {
		msg_post_int(kMSgThreeValveMotorRun,0);
	}

	g_three_valve_motor_direction_error_total = 0;
    g_three_valve_motor_encoder = 0;
    g_three_valve_motor_expect_encoder = expect_encoder;
    g_three_valve_motor_running_dir = dir;
	g_three_valve_motor_stop_cause = MOTOR_STOP_BY_USER;
	g_three_valve_motor_stop_handler = stop_handler;

	g_three_valve_motor_is_event_set = false;
	g_three_valve_motor_direction_error_count = 0;

    drv_three_valve_motor_brake();
    // enable power and disable sleep
    //drv_all_motor_pwr_enable();
    //drv_all_5v_pwr_enable();
    drv_three_valve_motor_pwr_enable();
	g_three_valve_motor_state=kMotorRun;

	//int speed=-1;
#if	THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
		g_three_valve_motor_soft_run_pwm_level = g_three_valve_motor_run_start_pwm_level;
#endif 

    if(dir == kForward){
        drv_three_valve_motor_move_forward(g_three_valve_motor_soft_run_pwm_level);
        motor_printfln("three-valve motor start forward, expect:%d", expect_encoder);
    }else if(dir == kBackward){
        drv_three_valve_motor_move_backward(g_three_valve_motor_soft_run_pwm_level);
        motor_printfln("three-valve motor start backward, expect:%d", expect_encoder);
    }else{
		ssz_assert_fail();
    }
    drv_three_valve_motor_sleep_disable();

    three_valve_motor_monitor_start();
}

void three_valve_motor_stop(int stop_cause)
{
	drv_three_valve_motor_brake();
	if (g_three_valve_motor_state == kMotorRun) {
		//wait some time to let it continue run by inertia
		ssz_delay_ms(50);
	}
    drv_three_valve_motor_sleep_enable();
    drv_three_valve_motor_pwr_disable();
    //drv_all_5v_pwr_disable();
    //drv_all_motor_pwr_disable();
    drv_three_valve_motor_inertial_stop();;
	if (g_three_valve_motor_state != kMotorStop) {
		g_three_valve_motor_state = kMotorStop;
		motor_printfln("three-valve motor stop, have run:%d, except:%d, cause:%d", g_three_valve_motor_encoder,
			g_three_valve_motor_expect_encoder, stop_cause);
		motor_printfln("error total have run:%d ",
		 g_three_valve_motor_direction_error_total );

		if (g_three_valve_motor_stop_handler) {
			g_three_valve_motor_stop_handler(g_three_valve_motor_encoder, stop_cause);
		}
		msg_post_int(kMSgThreeValveMotorStop, stop_cause);
	}
	three_valve_motor_monitor_stop();
}
int three_valve_motor_encoder_after_start()
{
	return g_three_valve_motor_encoder;
}
int three_valve_motor_expect_encoder()
{
	return g_three_valve_motor_expect_encoder;
}
MotorDirection three_valve_motor_direction()
{
    return g_three_valve_motor_running_dir;
}

bool three_valve_motor_is_running()
{
    bool ret;
    if(g_three_valve_motor_state == kMotorRun){
        ret = true;
    }else{
        ret = false;
    }
    return ret;
}


int three_valve_motor_stop_cause()
{
	return g_three_valve_motor_stop_cause;
}


void three_valve_motor_on_finish_target() {
	three_valve_motor_stop(MOTOR_STOP_BY_FINISH_EXPECT_TARGET);
}
void three_valve_motor_on_direction_wrong() {
	three_valve_motor_stop(MOTOR_STOP_BY_DIRECTION_WRONG);
	alarm_set(kAlarmThreeValveMotorDirectionErrorID);
}
void three_valve_motor_init()
{
	timer_set_handler(kTimerThreeValveMotorCheck, three_valve_motor_run_check);
	event_set_handler(kEventThreeValveMotorFinishTarget, three_valve_motor_on_finish_target);
	event_set_handler(kEventThreeValveMotorDirectionWrong, three_valve_motor_on_direction_wrong);
}

