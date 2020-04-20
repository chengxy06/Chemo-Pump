/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-10 jlli
* Initial revision.
*
************************************************/
#include "app_infusion_monitor.h"
#include "infusion_motor.h"
#include "three_valve_motor.h"
#include "alarm.h"
#include "msg.h"
#include "app_delivery.h"
#include "data.h"
#include "pressure_bubble_sensor.h"
#include "timer.h"
//#include "app_monitor_system.h"
#include "ssz_common.h"
#include "common.h"
#include "pill_box_install_detect.h"
#include "ssz_queue.h"
#include "param.h"
#include "mid_common.h"
#include "sys_power.h"
#include "param_internal.h"

/************************************************
* Declaration
************************************************/
#define INFUSION_MONITOR_CHECK_TIME_MS	           50
#define INFUSION_MONITOR_CHECK_WHEN_MOTOR_RUN_TIME_MS 50
#define INFUSION_MONITOR_CHECK_BUBBLE_INTERVAL_TIME_MS 500
#define INFUSION_MONITOR_CHECK_PRESSURE_DELAY_TIME_MS 1000

//输注开始时，判断阻塞的最大次数
#define MONITOR_OCCLUSION_MAX_COUNT_WHEN_RUN_STEP_BY_STEP 3   //一滴一滴输
#define MONITOR_OCCLUSION_MAX_COUNT_WHEN_CONTINUE_RUN 20 		//持续输

//气泡报警量
#define MONITOR_BUBBLE_ALARM_VOLUME_UL 1000
#define MONITOR_BUBBLE_ALARM_VOLUME_UL_LOW_SPEED 500

//一次气泡有效量,低于此量不计入,单位:uL
#define MONITOR_BUBBLE_VALID_VOLUME_UL 50 
//气泡时间
#define MONITOR_BUBBLE_TIME_MS_TO_ACTIVE_ALARM (15*60*1000)
#define MONITOR_BUBBLE_TIME_MS_TO_ACTIVE_ALARM_LOW_SPEED (60*60*1000)
//气泡队列最大值
#define MONITOR_BUBBLE_QUEUE_MAX_COUNT 100

//判断一次气泡所需要次数
#define MONITOR_BUBBLE_ONCE_CHECK_COUNT 10

#define MONITOR_INSTALL_NOT_RIGHT_MAX_COUNT 1

#define INFUSION_NEAR_EMPTY_TIME_SEC (3*60)

#define MONITOR_BATTERY_CHECK_SAMPLE 10
#define MONITOR_BATTERY_LOW_THAN_NORMAL_TIMES 3

typedef struct {
	int32_t check_time;//the time after infusion start
	int32_t run_encoder_count;//the run encoder counter from last check
	bool is_detected;
}BubbleCheckPoint;
/************************************************
* Variable 
************************************************/
//阻塞次数计数器
int g_monitor_occlusion_count = 0;
int g_monitor_pressure_adc_when_installed;
//true:用于排气和输注小药袋时
bool g_monitor_is_motor_run_continue;
int g_monitor_occlusion_max_count;


//气泡发现队列
BubbleCheckPoint g_monitor_bubbles_check_points[MONITOR_BUBBLE_QUEUE_MAX_COUNT];
SszDeque g_monitor_bubble_deque;
int32_t g_infusion_monitor_last_encoder_count_when_bubble_check;
int32_t g_infusion_monitor_last_bubble_check_time;

//一次检测气泡所用次数
int32_t g_infusion_monitor_bubble_check_count_for_filter;
bool g_infusion_monitor_bubble_check_result_for_filter[MONITOR_BUBBLE_ONCE_CHECK_COUNT];
//在马达转之后是否有有效检测，即是否满足一个过滤次数
bool g_infusion_monitor_bubble_is_have_valid_check_after_motor_run;

//安装错误计数器
int g_monitor_install_not_right_count = 0;

//是否已经通知过输液将为空报警
bool g_monitor_is_infusion_near_empty_notified;

//用于指示监视哪些东西
uint8_t g_infusion_monitor_flag;
//监视开始时间
int32_t g_infusion_start_time;
//监视开始后累计马达encoder数
int32_t g_infusion_monitor_motor_encoder_count_after_start;

//马达监视开始时间
int32_t g_motor_monitor_start_time;

int g_infusion_monitor_check_battery_low_sample[MONITOR_BATTERY_CHECK_SAMPLE];
int g_infusion_monitor_check_battery_low_index;
int g_infusion_monitor_check_battery_low_voltage;


int g_infusion_monitor_check_battery_empty_sample[MONITOR_BATTERY_CHECK_SAMPLE];
int g_infusion_monitor_check_battery_empty_index;
int g_infusion_monitor_check_battery_empty_voltage;

int g_infusion_monitor_battery_low_voltage_for_msg;
int g_infusion_monitor_battery_empty_voltage_for_msg;

int g_monitor_pressure_too_small_count;

/************************************************
* Function 
************************************************/

//判断阻塞
static void app_infusion_monitor_check_pressure(bool is_check_once_to_alarm)
{

	int occlusion_calibration_value;
	int occlusion_slope;
	int occlusion_adc_increase_of_one_drop;

	int current_pressure_adc_value;
	int pressure_offset;
	int current_pressure_value;
	int one_drop_pressure_value;
    LogVar log_vars[4];

    current_pressure_adc_value = pressure_bubble_sensor_get_pressure_sensor_ADC_average_value();
	pressure_offset = current_pressure_adc_value - g_monitor_pressure_adc_when_installed;
	occlusion_calibration_value = data_read_int(kDataOcclusionThreshold);
	occlusion_slope = data_read_int(kDataOcclusionSlope);
	occlusion_adc_increase_of_one_drop = data_read_int(kDataOcclusionIncreaseOfOneDrop);
	current_pressure_value = pressure_offset * occlusion_slope / 1000;
	one_drop_pressure_value = occlusion_adc_increase_of_one_drop * occlusion_slope / 1000;

	infusion_monitor_printfln("pressure=[%d]=[%d]mV=[%d]kPa,offset=[%d],one drop=[%d]kPa",
		current_pressure_adc_value,
		M_get_voltage_from_adc(current_pressure_adc_value), 
		current_pressure_value,
		pressure_offset,
		one_drop_pressure_value);

    //if(pressure_offset > occlusion_calibration_value){
	if(current_pressure_value+one_drop_pressure_value/2 > get_pressure_threshold()){
        g_monitor_occlusion_count++;  
		common_printfln("notice: the %d times meet pressure[%d][%d][%dkPa][%dkPa] is too big!", 
			g_monitor_occlusion_count, current_pressure_adc_value, pressure_offset, 
					current_pressure_value,
			one_drop_pressure_value);

        if(is_check_once_to_alarm || g_monitor_occlusion_count >= g_monitor_occlusion_max_count){
			common_printfln("occlusion!\n");
            alarm_set(kAlarmOcclusionID);
			log_vars[0].type = kLogVarInteger;
			log_vars[0].value = current_pressure_adc_value;
			log_vars[1].type = kLogVarInteger;
			log_vars[1].value = current_pressure_value;
			log_vars[2].type = kLogVarInteger;
			log_vars[2].value = g_monitor_pressure_adc_when_installed;
			log_vars[3].type = kLogVarInteger;
			log_vars[3].value = g_monitor_occlusion_count;
            record_log_add(kOperationLog, LOG_EVENT_TYPE_ALARM, 
                kLogEventOcclusionPressure,log_vars, ssz_array_size(log_vars));
			if (is_check_once_to_alarm) {
				g_infusion_motor_is_cannot_run = true;
			} else {
				infusion_motor_stop(MOTOR_STOP_CUSTOM_CAUSE_START + kAlarmOcclusionID);
			}

        }
	}
	else {
		if(g_monitor_is_motor_run_continue){
			//此时需要连续的值满足要求
			g_monitor_occlusion_count = 0;
		}
	}
}
static int32_t app_infusion_monitor_run_encoder_after_start() {
	if (infusion_motor_is_running()) {
		return g_infusion_monitor_motor_encoder_count_after_start+ infusion_motor_encoder_after_start();
	}
	else {
		return g_infusion_monitor_motor_encoder_count_after_start;
	}
}
static void app_infusion_monitor_check_bubble(bool is_use_filter)
{
	int32_t curr_encoder = app_infusion_monitor_run_encoder_after_start();
	if (curr_encoder <= g_infusion_monitor_last_encoder_count_when_bubble_check) {
		return;
	}
	bool is_bubble_exist = pressure_bubble_sensor_is_generate_bubble();
	infusion_monitor_printfln("bubble detect=[%d]", is_bubble_exist);

	//采集连续10次
	if(g_infusion_monitor_bubble_check_count_for_filter >= MONITOR_BUBBLE_ONCE_CHECK_COUNT){
		g_infusion_monitor_bubble_check_count_for_filter = 0;
	}
	g_infusion_monitor_bubble_check_result_for_filter[g_infusion_monitor_bubble_check_count_for_filter] =
		is_bubble_exist;
	g_infusion_monitor_bubble_check_count_for_filter++;
	if(is_use_filter && g_infusion_monitor_bubble_check_count_for_filter < MONITOR_BUBBLE_ONCE_CHECK_COUNT)
	{
		return;
	}
	//10次之后判断是否是气泡
	int sum = 0;
	for (int i = 0; i < g_infusion_monitor_bubble_check_count_for_filter; i++) {
		if(g_infusion_monitor_bubble_check_result_for_filter[i] == true) {
            sum++;
            if (sum >= 4) {
                is_bubble_exist = true;
            } else {
                is_bubble_exist = false;
            }
		}
	}
	infusion_monitor_printfln("%d times bubble filter finish, valid value=[%d]", 
		g_infusion_monitor_bubble_check_count_for_filter,
		is_bubble_exist);
	g_infusion_monitor_bubble_check_count_for_filter = 0;
	g_infusion_monitor_bubble_is_have_valid_check_after_motor_run = true;
	
	BubbleCheckPoint check_point;
	check_point.run_encoder_count = curr_encoder - g_infusion_monitor_last_encoder_count_when_bubble_check;
	check_point.is_detected = is_bubble_exist;
	check_point.check_time = ssz_tick_time_elapsed(g_infusion_start_time);
	if (ssz_queue_is_full(&g_monitor_bubble_deque)) {
		ssz_queue_pop(&g_monitor_bubble_deque);
	}
	ssz_queue_push(&g_monitor_bubble_deque, &check_point);
	g_infusion_monitor_last_encoder_count_when_bubble_check = curr_encoder;

	if (!is_bubble_exist) {
		return;
	}

	//check if the bubble arrive the alarm volume
	int32_t last_check_time = ((BubbleCheckPoint*)ssz_deque_back(&g_monitor_bubble_deque))->check_time;
	int bubble_total_encoder_count = 0;
	int bubble_continue_total_encoder_count = 0;
	int first_bubble_continue_total_encoder_count = 0; //used to print
    int32_t check_time_range = MONITOR_BUBBLE_TIME_MS_TO_ACTIVE_ALARM;
    int alarm_volume = MONITOR_BUBBLE_ALARM_VOLUME_UL;

    if(app_delivery_info(kNormalInfusion)->infusion_speed >= 5000){
        check_time_range = MONITOR_BUBBLE_TIME_MS_TO_ACTIVE_ALARM;
        alarm_volume = MONITOR_BUBBLE_ALARM_VOLUME_UL;
    } else {
        check_time_range = MONITOR_BUBBLE_TIME_MS_TO_ACTIVE_ALARM_LOW_SPEED;
        alarm_volume = MONITOR_BUBBLE_ALARM_VOLUME_UL_LOW_SPEED;
    }
    
	for (int pos = ssz_deque_rbegin(&g_monitor_bubble_deque); pos != -1;
		pos = ssz_deque_prev(&g_monitor_bubble_deque, pos)) {
		BubbleCheckPoint *p = (BubbleCheckPoint*)ssz_deque_at_pos(&g_monitor_bubble_deque, pos);
		//if it's interval big than alarm minutes, stop check
		if (last_check_time - p->check_time > check_time_range) {
			break;
		}
		if (p->is_detected) {
			bubble_continue_total_encoder_count += p->run_encoder_count;
			//check if big than 50uL
			if (encoder_to_dose(bubble_continue_total_encoder_count) >= MONITOR_BUBBLE_VALID_VOLUME_UL) {
				//it is big, need check all bubble
				if (encoder_to_dose(bubble_continue_total_encoder_count) +
					encoder_to_dose(bubble_total_encoder_count) >= alarm_volume) {
					//if all bubble is big than alarm volume, need set alarm
					common_printfln("have bubble!");
					alarm_set(kAlarmBubbleOrNoDoseID);
					break;
				}
			}
		}
		else {
			if (first_bubble_continue_total_encoder_count == 0) {
				first_bubble_continue_total_encoder_count = bubble_continue_total_encoder_count;
			}
			//if the previous bubble volume is big than 50uL, add to total bubble
			if (encoder_to_dose(bubble_continue_total_encoder_count) >= MONITOR_BUBBLE_VALID_VOLUME_UL) {
				bubble_total_encoder_count += bubble_continue_total_encoder_count;
			}
			bubble_continue_total_encoder_count = 0;
		}
	}


	//print
	if (encoder_to_dose(bubble_continue_total_encoder_count) >= MONITOR_BUBBLE_VALID_VOLUME_UL) {
		bubble_total_encoder_count += bubble_continue_total_encoder_count;
	}
	if (first_bubble_continue_total_encoder_count == 0) {
		first_bubble_continue_total_encoder_count = bubble_continue_total_encoder_count;
	}
	common_printfln("continue detect bubble[%duL]", encoder_to_dose(first_bubble_continue_total_encoder_count));
	common_printfln("total detect bubble[%duL] in last 15 minutes", encoder_to_dose(bubble_total_encoder_count));

}

static void app_infusion_monitor_check_near_empty() {
	if (!g_monitor_is_infusion_near_empty_notified) {
		if (app_delivery_state(kNormalInfusion)==kInfusionRunning&&
			( app_delivery_a_bag_rest_time_secs(kNormalInfusion)+
			app_delivery_b_bag_rest_time_secs(kNormalInfusion) )<=INFUSION_NEAR_EMPTY_TIME_SEC){
			common_printfln("infusion near empty!\n");
			alarm_set(kAlarmInfusionNearEmptyID);
			g_monitor_is_infusion_near_empty_notified = true;
		}
	}
}

static void app_infusion_monitor_check_install()
{
    //check pressure for installation
    if (pressure_bubble_sensor_get_pressure_sensor_ADC_average_value() < 100){
        g_monitor_pressure_too_small_count++;
    } else {
        g_monitor_pressure_too_small_count = 0;

    }

    if(g_monitor_pressure_too_small_count >= 5){
        g_monitor_pressure_too_small_count = 0;
        g_is_pill_box_installed = false;
        common_printfln("pressure < 50mV!\n");
    }


    if (!is_pill_box_install()) {
		g_monitor_install_not_right_count++;
		if (g_monitor_install_not_right_count >= MONITOR_INSTALL_NOT_RIGHT_MAX_COUNT) {
			common_printfln("install not right!\n");
			alarm_set(kAlarmInstallNotInPlace);
		}
	}
	else {
		g_monitor_install_not_right_count = 0;
	}
}

void app_infusion_monitor_check()
{
	if (g_infusion_monitor_flag&INFUSION_MONITOR_NEAR_EMPTY_FLAG) {
		app_infusion_monitor_check_near_empty();
	}
	if (g_infusion_monitor_flag&INFUSION_MONITOR_INSTALL_NOT_IN_PLACE_FLAG) {
		app_infusion_monitor_check_install();
	}
}

//infusion_flag: e.g. INFUSION_MONITOR_OCCLUSION_FLAG|INFUSION_MONITOR_BUBBLE_FLAG
void app_infusion_monitor_start(uint8_t monitor_flag)
{
	ssz_queue_init(&g_monitor_bubble_deque, &g_monitor_bubbles_check_points,
		ssz_array_size(g_monitor_bubbles_check_points), ssz_array_node_size(g_monitor_bubbles_check_points));
	g_infusion_monitor_last_encoder_count_when_bubble_check = 0;

		
	g_monitor_occlusion_count = 0;
	g_monitor_pressure_adc_when_installed = pressure_adc_when_installed();
	g_monitor_install_not_right_count = 0;
	g_monitor_is_infusion_near_empty_notified = false;

	g_infusion_monitor_flag = monitor_flag;
	g_infusion_start_time = ssz_tick_time_now();
	g_infusion_monitor_motor_encoder_count_after_start = 0;

	//enable monitor power
	//pressure_and_bubble_sensor_pwr_enable();
	//pill_box_install_sensor_left_pwr_enable();
	//pill_box_install_sensor_right_pwr_enable();

	//start monitor
    timer_set_handler(kTimerInfusionMonitor, app_infusion_monitor_check);
    timer_start_periodic_every(kTimerInfusionMonitor, INFUSION_MONITOR_CHECK_TIME_MS);
	common_printfln("infusion monitor start, flag[0x%X]", monitor_flag);
	common_printfln("pressure adc when installed: %d", g_monitor_pressure_adc_when_installed);
	app_infusion_monitor_check();


}

void app_infusion_monitor_stop()
{
	if (timer_is_actived(kTimerInfusionMonitor)) {
		timer_stop(kTimerInfusionMonitor);
		common_printfln("infusion monitor stop");

		//disable monitor power
		//pressure_and_bubble_sensor_pwr_disable();
		//pill_box_install_sensor_left_pwr_disable();
		//pill_box_install_sensor_right_pwr_disable();
	}
	g_infusion_monitor_flag = 0;
}
//用于指示监视哪些东西
uint8_t app_infusion_monitor_flag() {
	return g_infusion_monitor_flag;
}

void battery_low_alarm()
{
    if(!alarm_is_set(kAlarmBatteryExhaustionID)){
        g_current_battery_level = kBatteryLow;
        msg_post_two_param(kMsgBatteryStatusWhenMotorRun, kBatteryLow, g_infusion_monitor_battery_low_voltage_for_msg);
    }
}

void app_infusion_monitor_check_when_motor_run()
{
	if (timer_is_actived(kTimerInfusionMonitor)&& infusion_motor_is_running()) {
		if ((g_infusion_monitor_flag&INFUSION_MONITOR_BUBBLE_FLAG) && is_bubble_alarm_enable()) {
			app_infusion_monitor_check_bubble(true);
//			//每隔一段比较长的时间后再检测气泡
//			if (ssz_tick_time_elapsed(g_infusion_monitor_last_bubble_check_time) >= INFUSION_MONITOR_CHECK_BUBBLE_INTERVAL_TIME_MS) {
//				g_infusion_monitor_last_bubble_check_time = ssz_tick_time_now();
//				app_infusion_monitor_check_bubble();
//			}
		}
		
		if (g_infusion_monitor_flag&INFUSION_MONITOR_OCCLUSION_FLAG) {
			//延时一段时间后开始检测压力传感器, 因为输注时压力传感器波动很大, 所以在输注开始前检测一次
			//后面需要延时一段时候后才能继续检测
			if(g_monitor_is_motor_run_continue){ 
				if(ssz_tick_time_elapsed(g_motor_monitor_start_time)>=
				INFUSION_MONITOR_CHECK_PRESSURE_DELAY_TIME_MS){
					app_infusion_monitor_check_pressure(false);
				}
			}else{
				app_infusion_monitor_check_pressure(false);
			}
		}

        //check battey when motor run
        int battery = sys_power_battery_voltage();
        if (!is_use_extern_power()) {

            if (battery <= BATT_LEVEL_LOW_WHEN_RUN) 
                g_infusion_monitor_check_battery_low_voltage = battery;

            if (battery <= BATT_LEVEL_EMPTY_WHEN_RUN) 
                g_infusion_monitor_check_battery_empty_voltage = battery;

            int empty = 0, low = 0;
            for (int i = 0; i < MONITOR_BATTERY_CHECK_SAMPLE; i++) {
                empty += g_infusion_monitor_check_battery_empty_sample[i];
                low += g_infusion_monitor_check_battery_low_sample[i];
            
                if (empty >= MONITOR_BATTERY_LOW_THAN_NORMAL_TIMES) {
                    g_current_battery_level = kBatteryEmpty;
                    set_battery_status_empty_when_motor_run(true);
                    msg_post_two_param(kMsgBatteryStatusWhenMotorRun, kBatteryEmpty, g_infusion_monitor_battery_empty_voltage_for_msg);
                    ssz_mem_zero(g_infusion_monitor_check_battery_empty_sample, sizeof(g_infusion_monitor_check_battery_empty_sample));
                    break;
                }
                if (low >= MONITOR_BATTERY_LOW_THAN_NORMAL_TIMES) {
                    set_battery_status_low_when_motor_run(true);
                    timer_set_handler(kTimerDelayBeforeBatteryLowAlarm, battery_low_alarm);
			        timer_start_oneshot_after(kTimerDelayBeforeBatteryLowAlarm,
				        DELAY_BEFORE_BATTERY_LOW_ALARM_MS);
                    ssz_mem_zero(g_infusion_monitor_check_battery_low_sample, sizeof(g_infusion_monitor_check_battery_low_sample));
                    break;
                }
            }
        }
	}
}

void app_infusion_monitor_on_before_motor_start(MsgParam param)
{
	g_monitor_occlusion_count = 0;
	if (g_infusion_monitor_flag&INFUSION_MONITOR_OCCLUSION_FLAG) {
		app_infusion_monitor_check_pressure(true);
	}

}

void app_infusion_monitor_on_motor_start(MsgParam param)
{
	InfusionType infusion_type = app_delivery_curr_running_infusion();
	if(infusion_type == kClearAir ||
		infusion_type == kCleanTube ){
		g_monitor_is_motor_run_continue = true;
	}else if(infusion_type == kNormalInfusion && app_delivery_curr_bag(infusion_type)==kBagBMeds){
		g_monitor_is_motor_run_continue = true;
	}else{
		g_monitor_is_motor_run_continue = false;
	}

    g_infusion_monitor_check_battery_low_voltage = 3000;
    g_infusion_monitor_check_battery_empty_voltage = 3000;
    
	//pressure_and_bubble_sensor_pwr_enable(); 移到输注电机模块
	timer_set_handler(kTimerInfusionMotorMonitor, app_infusion_monitor_check_when_motor_run);
	timer_start_periodic_every(kTimerInfusionMotorMonitor, INFUSION_MONITOR_CHECK_WHEN_MOTOR_RUN_TIME_MS);
	g_infusion_monitor_last_bubble_check_time = ssz_tick_time_now();
	g_motor_monitor_start_time = ssz_tick_time_now();
	
	g_infusion_monitor_bubble_is_have_valid_check_after_motor_run = 0;
	g_infusion_monitor_bubble_check_count_for_filter = 0;


	if(g_monitor_is_motor_run_continue){
		g_monitor_occlusion_max_count = MONITOR_OCCLUSION_MAX_COUNT_WHEN_CONTINUE_RUN;
	}else{
		g_monitor_occlusion_max_count = MONITOR_OCCLUSION_MAX_COUNT_WHEN_RUN_STEP_BY_STEP;
	}
}

void app_infusion_monitor_on_motor_stop(MsgParam param)
{
	timer_stop(kTimerInfusionMotorMonitor);
	if (timer_is_actived(kTimerInfusionMonitor) && is_bubble_alarm_enable()) {
		g_infusion_monitor_motor_encoder_count_after_start += param.int_param;
		if(g_infusion_monitor_flag&INFUSION_MONITOR_BUBBLE_FLAG){
			if(!g_infusion_monitor_bubble_is_have_valid_check_after_motor_run){
				//如果还没有有效检测，则强制进行有效检测
				app_infusion_monitor_check_bubble(false);
			}
		}
	}
	if(system_mode()== kSystemModeNormal){
		//pressure_and_bubble_sensor_pwr_disable();
	}

    if (g_infusion_monitor_check_battery_low_index < MONITOR_BATTERY_CHECK_SAMPLE){
        if (g_infusion_monitor_check_battery_low_voltage <= BATT_LEVEL_LOW_WHEN_RUN ){
            g_infusion_monitor_check_battery_low_sample[g_infusion_monitor_check_battery_low_index] = 1;
            g_infusion_monitor_battery_low_voltage_for_msg = g_infusion_monitor_check_battery_low_voltage;
        } else {
            g_infusion_monitor_check_battery_low_sample[g_infusion_monitor_check_battery_low_index] = 0;
        }
        g_infusion_monitor_check_battery_low_index++;
    }
    if (g_infusion_monitor_check_battery_low_index >= MONITOR_BATTERY_CHECK_SAMPLE || g_infusion_monitor_check_battery_low_index < 0)
        g_infusion_monitor_check_battery_low_index = 0;


    if (g_infusion_monitor_check_battery_empty_index < MONITOR_BATTERY_CHECK_SAMPLE){
        if (g_infusion_monitor_check_battery_empty_voltage <= BATT_LEVEL_EMPTY_WHEN_RUN ) {
            g_infusion_monitor_check_battery_empty_sample[g_infusion_monitor_check_battery_empty_index] = 1;
            g_infusion_monitor_battery_empty_voltage_for_msg = g_infusion_monitor_check_battery_empty_voltage;
        } else {
            g_infusion_monitor_check_battery_empty_sample[g_infusion_monitor_check_battery_empty_index] = 0;
        }
        g_infusion_monitor_check_battery_empty_index++;
    }
    if (g_infusion_monitor_check_battery_empty_index >= MONITOR_BATTERY_CHECK_SAMPLE || g_infusion_monitor_check_battery_empty_index < 0)
        g_infusion_monitor_check_battery_empty_index = 0;

}

void app_infusion_monitor_init() {
	//handle motor start and stop msg
	msg_set_handler(kMSgBeforeInfusionMotorRun, app_infusion_monitor_on_before_motor_start);
	msg_set_handler(kMSgInfusionMotorRun, app_infusion_monitor_on_motor_start);
	msg_set_handler(kMSgInfusionMotorStop, app_infusion_monitor_on_motor_stop);
}
