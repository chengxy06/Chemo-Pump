/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-19 dczhang
* Initial revision.
*
************************************************/
#include "app_system_monitor.h"
#include "vibrater_motor.h"
#include "infusion_motor.h"
#include "sys_power.h"
#include "msg.h"
#include "alarm.h"
#include "drv_isd2360.h"
#include "ssz_common.h"
#include "app_delivery.h"
#include "mid_common.h"
#include "key.h"
#include "pill_box_install_detect.h"
#include "param_internal.h"
#include "param.h"
#include "common.h"
#include "app_common.h"
#include "screen.h"
#include "pressure_bubble_sensor.h"
#include "param_internal.h"
/************************************************
* Declaration
************************************************/
#define SYSTEM_MONITOR_CHECK_TIME_MS      100
#define SYSTEM_MONITOR_INSTALL_STATE_CHANGE_COUNT 2

/************************************************
* Variable 
************************************************/
static int g_monitor_last_battery_level= kBatteryLevelMax;
static int g_monitor_last_battery_level_hold_time;


//static bool g_monitor_last_pill_box_installed_state = false;
static int g_monitor_last_pill_box_installed_state_hold_count = 0;
int g_monitor_no_operate_time_ms = NO_OPERATE_TIME_MS;
bool g_is_sleep_allowed = true;
static int g_system_monitor_pressure_too_small_count;

/************************************************
* Function 
************************************************/
void app_system_monitor_check_battery_status()
{
	bool is_use_extern_power = false;

	//检测是否插入适配器
	is_use_extern_power = sys_power_is_adapter_insert();
	if (g_current_is_use_extern_power != is_use_extern_power) {
		g_current_is_use_extern_power = is_use_extern_power;
		msg_post_int(kMsgExternPowerStatusChange, is_use_extern_power);
	}

	//check if exist battery
//	bool is_battery_exist = sys_power_is_battery_exist();
//	if (g_current_battery_is_exist != is_battery_exist) {
//		g_current_battery_is_exist = is_battery_exist;
//		msg_post_int(kMsgBatteryStatusChange, is_battery_exist);
//	}
//
//	if (is_use_extern_power) {
//		//设置电量为未知
//		g_current_battery_level = kBatteryFour;
//		g_monitor_last_battery_level = kBatteryLevelMax;
//		g_monitor_last_battery_level_hold_time = 0;
//		return;
//	}

    //motor,vibrator,voice 
    if((g_vibrator_motor_is_running == false)){
		BatteryLevel new_battery_level;
		BatteryEnvironment power_environment = kBatteryAtNormalPower;
		//if (screen_current_id() == kScreenAlarm){
		//	power_environment = kBatteryAtBigPower;
		//}else{
		//	power_environment = kBatteryAtNormalPower;
		//}
		
        new_battery_level = sys_power_battery_level(power_environment);
        //printf("level: %d\n",new_battery_level);
		if (new_battery_level != g_monitor_last_battery_level) {
			g_monitor_last_battery_level = new_battery_level;
			g_monitor_last_battery_level_hold_time = SYSTEM_MONITOR_CHECK_TIME_MS;
		}
		else {
			g_monitor_last_battery_level_hold_time += SYSTEM_MONITOR_CHECK_TIME_MS;
		}

		if (g_monitor_last_battery_level_hold_time >= 2000)
		{
			g_monitor_last_battery_level_hold_time = 0;
			//if battery level change, set the msg
			if (new_battery_level != g_current_battery_level)
			{
				//only when AC is connect or the battery level is high than low, 
				// it can change free.
				// or it can only change to lower
				if (g_current_battery_level > kBatteryLow || g_current_is_use_extern_power) {
					g_current_battery_level = new_battery_level;
					msg_post_int(kMsgBatteryLevelChange, new_battery_level);
				}
				else if (new_battery_level < g_current_battery_level) {
					g_current_battery_level = new_battery_level;
					msg_post_int(kMsgBatteryLevelChange, new_battery_level);
				}
			}
		}
    }
}

void app_system_monitor_check_key_stuck() {
	//check if the keys is stuck
	KeyID keys[] = { kKeyOK, kKeyBack, kKeyStart};
	//check key stuck
	for (int i = 0; i < ssz_array_size(keys); i++)
	{
		if (key_hold_time(keys[i])>= KEY_STUCK_TIME_MS)
		{
			if(!alarm_is_set(kAlarmKeyStuckID)){
				record_log_add_with_one_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
					kLogEventKeyStuck,
					i);
				alarm_set(kAlarmKeyStuckID);
			}
			break;
		}
	}
}
void app_system_monitor_no_operate_timer_out() {
	if(screen_current_id()!=kScreenSleep && g_is_sleep_allowed){
		common_printfln("no operate, try to close screen");
		msg_post_int(kMsgCloseDisplay, 0);
	}
}

void app_system_monitor_check_no_operate() {
	if(is_allow_check_no_operate()){
		if (!timer_is_actived(kTimerNoOperate)&& g_monitor_no_operate_time_ms>0) {
			timer_start_oneshot_after(kTimerNoOperate, g_monitor_no_operate_time_ms);
		}
	}
	else {
		timer_stop(kTimerNoOperate);
	}
}
void app_system_monitor_check_pill_box_install() {
	bool is_installed = false;
	if (pill_box_install_left_detect() && pill_box_install_right_detect()) {
		is_installed = true;
	}

	if (g_is_pill_box_installed != is_installed) {
		g_monitor_last_pill_box_installed_state_hold_count++;
		if (g_monitor_last_pill_box_installed_state_hold_count>= SYSTEM_MONITOR_INSTALL_STATE_CHANGE_COUNT) {
			g_is_pill_box_installed = is_installed;

			msg_post_int(kMsgInstallStateChanged, is_installed);
		}
	}
	else {
		g_monitor_last_pill_box_installed_state_hold_count = 0;
	}

    if (pressure_bubble_sensor_get_pressure_sensor_ADC_average_value() < 100){
        g_system_monitor_pressure_too_small_count++;
    } else {
        g_system_monitor_pressure_too_small_count = 0;
    }

    if(g_system_monitor_pressure_too_small_count >= 5){
        g_system_monitor_pressure_too_small_count = 0;
        g_is_pill_box_installed = false;
    }

    
}
void app_system_monitor_check()
{
    app_system_monitor_check_battery_status();
	app_system_monitor_check_key_stuck();
	app_system_monitor_check_no_operate();
	app_system_monitor_check_pill_box_install();

//	int current_pressure_value;
//	int pressure_offset;
//	current_pressure_value = pressure_bubble_sensor_get_pressure_sensor_ADC_average_value();
//	pressure_offset = current_pressure_value - pressure_adc_when_installed();
//	ssz_traceln("pressure=[%d]=[%d]mV,offset=[%d]",current_pressure_value,
//		M_get_voltage_from_adc(current_pressure_value), pressure_offset);
}

void app_system_monitor_start()
{
	g_monitor_last_battery_level = kBatteryLevelMax;
	g_monitor_last_battery_level_hold_time = 0;

	//enable adc power
	sys_power_ADC_enable();
    pressure_and_bubble_sensor_pwr_enable();
	
	//enable install check
	pill_box_install_sensor_left_pwr_enable();
	pill_box_install_sensor_right_pwr_enable();

    //timer_set_handler(kTimerSystemMonitor, app_system_monitor_check);
    //timer_start_periodic_every(kTimerSystemMonitor, SYSTEM_MONITOR_CHECK_TIME_MS);
	//mer_set_handler(kTimerNoOperate, app_system_monitor_no_operate_timer_out);

	app_system_monitor_check();
}

void app_system_monitor_stop()
{
	//disable adc power
	sys_power_ADC_disable();
	
    timer_stop(kTimerSystemMonitor);
	timer_stop(kTimerNoOperate);
}

void app_system_monitor_set_no_operate_time(int no_operate_time_ms) {
	g_monitor_no_operate_time_ms = no_operate_time_ms;
	timer_stop(kTimerNoOperate);
	if (timer_is_actived(kTimerSystemMonitor)) {
		app_system_monitor_check_no_operate();
	}
}
void app_system_monitor_set_to_default_no_operate_time() {
	app_system_monitor_set_no_operate_time(NO_OPERATE_TIME_MS);
}
