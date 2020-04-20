/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-10 xqzhao
* Initial revision.
*
************************************************/
#include "app_common.h"
#include "ssz_common.h"

#include "ui_view.h"
#include "ui_statusbar.h"
#include "ui_define.h"
#include "mid_common.h"
#include "screen.h"
#include "data.h"
#include "scr_factory_mode.h"
#include "app_home.h"
#include "key.h"
#include "record_log.h"
#include "app_delivery.h"
#include "ui_common.h"
#include "app_mcu_comm.h"
#include "infusion_motor.h"
#include "three_valve_motor.h"
#include "alarm_play.h"
#include "param_internal.h"
#include "mid_common.h"
#include "pressure_bubble_sensor.h"
#include "app_mcu_monitor.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
UserSetInfusionPara g_user_set_infusion_para;
//bool g_is_user_set_new_task;
static bool g_is_keypad_locked = false;
static bool g_is_allow_sleep = true;
static bool g_is_watchdog_enable = false;
static bool g_is_auto_power_off = false;
static int g_sleep_before_wakeup_s = 1;

#if TEST_LIFETIME_ENABLE
bool g_is_testing_lifetime = false;
InfusionParam g_test_lifetime_infusion_param;
#endif
/************************************************
* Function 
************************************************/

void set_all_user_infusion_para(UserSetInfusionParaType *infusion_data)
{
    memcpy(&g_user_set_infusion_para, infusion_data, sizeof(g_user_set_infusion_para));
}


void set_user_infusion_para(UserSetInfusionParaType type, int set_data)
{
    switch (type){
        case kTotalDose:
            g_user_set_infusion_para.total_dose=set_data;
        break;
        case kCleanTubeDose:
            g_user_set_infusion_para.meds_b_total_dose =set_data;
        break;
        case kLimitInfusionSpeed:
            g_user_set_infusion_para.limit_infusion_speed=set_data;
        break;
        case kInfusionSpeed:
            g_user_set_infusion_para.infusion_speed=set_data;
        break;
        case kLimitInfusionDuration:
            g_user_set_infusion_para.limit_infusion_duration=set_data;
        break;
        case kInfusionDuration:
            g_user_set_infusion_para.infusion_duration=set_data;
        break;
        case kIsPeekInfusionMode:
            g_user_set_infusion_para.is_peek_infusion_mode=(bool)set_data;
        break;
        case kPeekInfusionStartTimePoint:
            g_user_set_infusion_para.peek_infusion_start_time_point=set_data;
        break;
        case kPeekInfusionEndTimePoint:
            g_user_set_infusion_para.peek_infusion_end_time_point=set_data;
        break;
        case kPeekInfusionSpeed:
            g_user_set_infusion_para.peek_infusion_speed=set_data;
        break;
        default:
            break;
    }
}

int get_user_infusion_para(UserSetInfusionParaType type)
{
    int ret;
    switch (type){
        case kTotalDose:
            ret=g_user_set_infusion_para.total_dose;
        break;
        case kCleanTubeDose:
            ret=g_user_set_infusion_para.meds_b_total_dose;
        break;
        case kLimitInfusionSpeed:
            ret=g_user_set_infusion_para.limit_infusion_speed;
        break;
        case kInfusionSpeed:
            ret=g_user_set_infusion_para.infusion_speed;
        break;
        case kLimitInfusionDuration:
            ret=g_user_set_infusion_para.limit_infusion_duration;
        break;
        case kInfusionDuration:
            ret=g_user_set_infusion_para.infusion_duration;
        break;        
        case kIsPeekInfusionMode:
            ret=g_user_set_infusion_para.is_peek_infusion_mode;
        break;
        case kPeekInfusionStartTimePoint:
            ret=g_user_set_infusion_para.peek_infusion_start_time_point;
        break;
        case kPeekInfusionEndTimePoint:
            ret=g_user_set_infusion_para.peek_infusion_end_time_point;
        break;
        case kPeekInfusionSpeed:
            ret=g_user_set_infusion_para.peek_infusion_speed;
        break;
        default:
            break;
    }
    return ret;
}
void set_sleep_secs_before_wakeup(int sec) {
	g_sleep_before_wakeup_s = sec;
}
int get_sleep_secs_before_wakeup(void) {
	return g_sleep_before_wakeup_s;
}

void power_off()
{
	data_flush();
	record_log_add_info_with_data(kHistoryLog, kLogEventPowerOff, NULL, 0);
    if(is_auto_power_off())
            record_log_add_info_with_data(kOperationLog, kLogEventAutoPowerOff, NULL, 0);
    app_mcu_send_to_slave(COMM_POWER_OFF,0,0);
	sys_power_disable();
}

bool is_auto_power_off() {
	return g_is_auto_power_off;
}

void set_auto_power_off(bool is_auto_power_off) {
	g_is_auto_power_off = is_auto_power_off;
}


void enter_factory_mode()
{
	watchdog_stop();
	set_system_mode(kSystemModeFactory);
	scr_factory_mode_enter();
}

void exit_factory_mode()
{
	watchdog_start();
	set_system_mode(kSystemModeNormal);
	screen_go_back_to_home();
}

bool is_allow_check_no_operate(void)
{
	if (system_mode()==kSystemModeFactory ||
		system_mode() == kSystemModeSleep ||
		key_is_any_key_pressed_except(kKeyIDMax)||
		app_delivery_state(kClearAir)==kInfusionRunning ||
		app_delivery_state(kCleanTube) == kInfusionRunning ||
		alarm_highest_priority_level()>= kAlarmLevelLowest
		)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void lock_keypad() {
	g_is_keypad_locked = true;
	statusbar()->show_icon(kStatusbarIconOrderLock, get_image(kImgLock));
}
void unlock_keypad() {
	g_is_keypad_locked = false;
	statusbar()->hide_icon(kStatusbarIconOrderLock);
}
bool is_keypad_locked() {
	return g_is_keypad_locked;
}
void set_sleep_allow(bool is_allow_sleep) {
	g_is_allow_sleep = is_allow_sleep;
}
bool is_sleep_allow() {
	if (infusion_motor_is_running() ||
		three_valve_motor_is_running() ||
		alarm_is_play_beep_several_times() || 
		!g_is_allow_sleep ||
		alarm_highest_priority_level() >= kAlarmLevelLowest
		) {
		return false;
	}
	else {
		return true;
	}
}

void prepare_pressure_adc_before_infusion_start(){
	if(!is_pressure_adc_ready_after_installed() && is_pill_box_install()){
		if (!pressure_and_bubble_sensor_is_pwr_enable()) {
			pressure_and_bubble_sensor_pwr_enable();
			ssz_delay_ms(5);
		}
		g_pressure_adc_when_installed = pressure_bubble_sensor_get_pressure_sensor_ADC_average_value();
		g_is_pressure_adc_ready_when_installed = true;
		common_printfln("installed pressure[%d] before infusion start", g_pressure_adc_when_installed);
	}
}

void delay_ms_without_watchdog(int delay_ms) {
	ssz_delay_ms_with_clear_watchdog(delay_ms);
	app_mcu_monitor_stop();
}

void watchdog_start(){
	g_is_watchdog_enable = true;
	app_mcu_send_to_slave(COMM_SLAVER_MCU_START_DOG, 0, 0);
}
void watchdog_stop(){
	g_is_watchdog_enable = false;
	app_mcu_send_to_slave(COMM_SLAVER_MCU_STOP_DOG,0,0);
	app_mcu_monitor_stop();
}
bool watchdog_is_enable(){
	return g_is_watchdog_enable;
}
void watchdog_clear(){
	app_mcu_send_to_slave(COMM_SLAVER_MCU_CLEAR_DOG, 0, 0);
	app_mcu_monitor_start();
}
void watchdog_set_expired_time(int expired_time){
	app_mcu_send_to_slave(COMM_SLAVER_MCU_SET_DOG_EXPIRED_TIME,expired_time,0);
	app_mcu_monitor_set_watchdog_expired_time(expired_time);
}

//calc the a and b of the line, the line equation is: y=ax+b
//refer http://www.fjptsz.com/xxjs/xjw/rj/117/04.htm
//refer http://wenku.baidu.com/link?url=DS_yeT9HH3sZe_SYidtmATrLFTTRM92qdlBWtbqz86GymGnyorsoyFJASrdt1zFQ6fjnjvjlSyb_iwC9BpQDsSlxPod8dp3u-qWs_PhwuG7
LineEquationPara fit_line_by_data(int x[], int y[], int count)
{
    LineEquationPara ret;
    double x_total=0; //=x1+x2+x3...+xn
    double y_total=0; //=y1+y2+y3...+yn
    double xy_total=0; //x1*y1+x2*y2+x3*y3...+xn*yn
    double x_square_total=0; //x1*x1+x2*x2+x3*x3...+xn*xn
    int i;

    //calc the total 
    for(i=0; i<count; i++)
    {
        x_total += x[i];
        y_total += y[i];
        xy_total += x[i]*y[i];
        x_square_total += x[i]*x[i];
    }

    ret.a = (double)(count*xy_total-x_total*y_total)/(count*x_square_total-x_total*x_total)*1000;
    ret.b = ((double)y_total*x_square_total-(double)x_total*xy_total)/(count*x_square_total-x_total*x_total)*1000;

    return ret;
    
}


