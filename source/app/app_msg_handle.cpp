/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-21 xqzhao
* Initial revision.
*
************************************************/
#include "app_msg_handle.h"
#include "msg.h"
#include "ui_common.h"
#include "scr_sleep.h"
#include "screen.h"
#include "pressure_bubble_sensor.h"
#include "param_internal.h"
#include "alarm.h"
#include "mid_common.h"
#include "common.h"
#include "app_delivery.h"
#include "app_common.h"
#include "record_log.h"
#include "sys_power.h"
#include "common.h"
#include "ssz_time_utility.h"
#include "do_before_main_loop.h"
#include "app_sleep.h"
#include "key.h"
#include "infusion_motor.h"
#include "app_mcu_monitor.h"
#include "led.h"
#include "param.h"

/************************************************
* Declaration
************************************************/
//提前醒来秒数, 提前醒来可以防止时间走快了而导致没有输注
#define WAKE_UP_TIME_BEFORE_SECS 2

#define READ_PRESSURE_VALUE_AFTER_EN_POWER_MS 2000

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
void app_update_battery_and_ac_icon() {
	statusbar()->set_battery(is_use_extern_power(), is_battery_exist(), battery_level());
	if (is_use_extern_power()) {
		statusbar()->show_icon(kStatusbarIconOrderAC, get_image(kImgAc));
	} else {
		statusbar()->hide_icon(kStatusbarIconOrderAC);
	}
}
void app_check_battery_alarm() {
	BatteryLevel new_battery_level = battery_level();
    LogVar log_var;

    if (!is_use_extern_power()) {
		if (new_battery_level == kBatteryLow) {
			alarm_set(kAlarmLowBatteryID);
            log_var.type = kLogVarInteger;
            log_var.value = sys_power_battery_voltage();
            record_log_add(kOperationLog, LOG_EVENT_TYPE_ALARM, 
                kLogEventBatteryLow,&log_var, 1);

            //statusbar()->set_battery(is_use_extern_power(), is_battery_exist(), kBatteryLow);
		} else if (new_battery_level == kBatteryEmpty) {
			alarm_set(kAlarmBatteryExhaustionID);
            log_var.type = kLogVarInteger;
            log_var.value = sys_power_battery_voltage();
            record_log_add(kOperationLog, LOG_EVENT_TYPE_ALARM,
                kLogEventBatteryExhausted,&log_var, 1);
            set_auto_power_off(true);
			timer_set_handler(kTimerBatteryEmptyDelayToPoweroff, power_off);
			timer_start_oneshot_after(kTimerBatteryEmptyDelayToPoweroff,
				AUTO_POWER_OFF_TIME_MS_AFTER_BATTERY_EMPTY);
            //statusbar()->set_battery(is_use_extern_power(), is_battery_exist(), kBatteryEmpty);
		}
	}else{
	    if(new_battery_level == kBatteryNotExist)
            alarm_set(kAlarmBatteryNotExist);
    }
}
void app_on_battery_level_changed(MsgParam param) {
	//BatteryLevel new_battery_level = (BatteryLevel)param.int_param;

  	app_check_battery_alarm();
    app_update_battery_and_ac_icon();
	
	//if (new_battery_level == kBatteryNotExist) {
	//alarm_set(kAlarmBatteryNotExist);
	//}
}

void app_on_battery_level_changed_when_motor_run(MsgParam param) {
	BatteryLevel new_battery_level = (BatteryLevel)param.int16_param1;
    int32_t battery = (BatteryLevel)param.int16_param2;
  	LogVar log_var;

    if (!is_use_extern_power()) {
		if (new_battery_level == kBatteryLow) {
			alarm_set(kAlarmLowBatteryID);
            log_var.type = kLogVarInteger;
            log_var.value = battery;
            record_log_add(kOperationLog, LOG_EVENT_TYPE_ALARM, 
                kLogEventBatteryLow,&log_var, 1);
            statusbar()->set_battery(is_use_extern_power(), is_battery_exist(), kBatteryLow);
		} else if (new_battery_level == kBatteryEmpty) {
			alarm_set(kAlarmBatteryExhaustionID);
            log_var.type = kLogVarInteger;
            log_var.value = battery;
            record_log_add(kOperationLog, LOG_EVENT_TYPE_ALARM,
                kLogEventBatteryExhausted,&log_var, 1);
            set_auto_power_off(true);
			timer_set_handler(kTimerBatteryEmptyDelayToPoweroff, power_off);
			timer_start_oneshot_after(kTimerBatteryEmptyDelayToPoweroff,
				AUTO_POWER_OFF_TIME_MS_AFTER_BATTERY_EMPTY);
            statusbar()->set_battery(is_use_extern_power(), is_battery_exist(), kBatteryEmpty);
		}
	}
}

void app_on_extern_power_changed(MsgParam param) {
	if (param.int_param == 1) {
		//如果使用外部电源, 关闭低电量等报警
		alarm_clear(kAlarmAdapterDisconnectedID);
		alarm_clear(kAlarmLowBatteryID);
		alarm_clear(kAlarmBatteryExhaustionID);
		//停止没电自动关机
		timer_stop(kTimerBatteryEmptyDelayToPoweroff);
		//如果没有报警,亮起绿灯
		if(alarm_highest_priority_level()<kAlarmLevelLowest){
			led_turn_on(kLedGreen);
		}
	}
	else {
		//如果没有报警,关闭所有灯
		if (alarm_highest_priority_level()<kAlarmLevelLowest){
			led_turn_off();
		}

		alarm_set(kAlarmAdapterDisconnectedID);
		app_check_battery_alarm();
	}

	app_update_battery_and_ac_icon();
}

void app_on_time_changed(MsgParam param) {
	if(ui_view_desktop()->get_descendant(kUIStatusBar)){
		statusbar()->set_time(ssz_time_now());
	}
}
void app_get_pressure_value_after_install() {
	//ssz_delay_ms(100);
	if (pressure_and_bubble_sensor_is_pwr_enable()) {
		g_pressure_adc_when_installed = pressure_bubble_sensor_get_pressure_sensor_ADC_average_value();
		common_printfln("installed, pressure[%d]", g_pressure_adc_when_installed);
		if (!infusion_motor_is_running()&&system_mode()== kSystemModeNormal) {
			//if motor not run, disable power
			pressure_and_bubble_sensor_pwr_disable();
		}
	}
	else if(is_pill_box_install()){
		pressure_and_bubble_sensor_pwr_enable();
		timer_start_oneshot_after(kTimerGetPressureAfterInstall, READ_PRESSURE_VALUE_AFTER_EN_POWER_MS);
	}
}
void app_on_install_state_changed(MsgParam param) {
	if (param.int_param == 1) {
		//pressure_and_bubble_sensor_pwr_enable();
		//timer_set_handler(kTimerGetPressureAfterInstall, app_get_pressure_value_after_install);
		//timer_start_oneshot_after(kTimerGetPressureAfterInstall, READ_PRESSURE_VALUE_AFTER_EN_POWER_MS);
	}else{
		g_is_pressure_adc_ready_when_installed = false;
	}
}
void app_on_close_display(MsgParam param) {
	screen_delete_by_id(kScreenSleep);
	scr_sleep_enter();
	if (app_delivery_state(kNormalInfusion)==kInfusionRunning) {
		lock_keypad();
	}
}
void app_enable_sleep() {
	//printf("enable at %s\n", ssz_time_now());
	set_sleep_allow(true);
}
void app_on_go_to_sleep(MsgParam param) {
	if (!is_sleep_allow()) {
		return;
	}
	int sleep_secs;
	int32_t wake_up_time_secs = ssz_time_now_seconds();
	SszDateTime wake_up_time;

	//common_printfln("go to sleep");

	sleep_secs = app_delivery_next_infuse_point_diff_ms_from_now()/1000;
	if (sleep_secs == INT32_MAX/1000)
	{
		wake_up_time_secs = -1;
		common_printfln("will sleep forever");
	}
	else
	{
		sleep_secs -= get_sleep_secs_before_wakeup();
		if (sleep_secs > 0)
		{
			wake_up_time_secs += sleep_secs;
			ssz_seconds_to_time(wake_up_time_secs, &wake_up_time);
			common_printfln("will sleep %ds and will wake up at %02d:%02d:%02d", sleep_secs,
				wake_up_time.hour, wake_up_time.minute, wake_up_time.second);
		}
		else
		{
			//common_printfln("only left %ds to delivery, so not sleep", sleep_secs);
		}
	}

	if (sleep_secs > 0)
	{
		bool is_wakeup_by_time_out = false;
		//stop monitors and scan
		stop_system_scan();
		watchdog_stop();
		
		//execute sleep
		common_printfln("sleep at %s", ssz_time_now_str());
		if (wake_up_time_secs == -1) {
			app_sleep_enter();
		}
		else{
			is_wakeup_by_time_out = app_sleep_enter_and_exit_at(&wake_up_time);
		}
		
		//wake up
		//start monitor and scan
		start_system_scan();
		watchdog_start();
		common_printfln("wakeup at %s", ssz_time_now_str());
		
		//temp disable sleep for some time to wait system do something
		set_sleep_allow(false);
		timer_set_handler(kTimerEnableSleep, app_enable_sleep);
		timer_start_oneshot_after(kTimerEnableSleep, 40);

		if (is_wakeup_by_time_out) {
			//wait delivery to run
			timer_start_oneshot_after(kTimerEnableSleep, WAKE_UP_TIME_BEFORE_SECS*2 * 1000);
			//enter sleep screen
			scr_sleep_enter();

			//print info
			common_printfln("the destination wake up time is at %02d:%02d:%02d",
				wake_up_time.hour, wake_up_time.minute, wake_up_time.second);
			InfusionTime infuse_time = app_delivery_next_infuse_point_time();
			SszDateTime tmp_time;
			ssz_seconds_to_time(infuse_time.rtc_seconds, &tmp_time);
			common_printfln("will delivery at time: %02d:%02d:%02d",
				tmp_time.hour, tmp_time.minute, tmp_time.second);

			//check if delivery time passed for the sleep timer is not right
			int diff_secs = app_delivery_next_infuse_point_diff_ms_from_now()/1000;
			//if the time passed in one minute, delivery at once
			if (diff_secs <= 0 && diff_secs > -60)
			{
				app_delivery_infuse_next_one_point_at_once();
			}
		}
		else {
			//because user need press confirm key to wake up, so set confirm key as hold
			key_set_hold_time(kKeyOK, KEY_HOLD_TIME_MS);
			//close sleep screen
			screen_delete_by_id(kScreenSleep);
		}

	}
}

void app_on_alarm_voice_mute(MsgParam param) {
	//statusbar()->show_icon(kStatusbarIconOrderMute, get_image(kImgMute));
}
void app_on_alarm_voice_unmute(MsgParam param) {
	//statusbar()->hide_icon(kStatusbarIconOrderMute);
}

//register key monitor event
void app_msg_handle_init()
{
	msg_set_handler(kMsgBatteryLevelChange, app_on_battery_level_changed);
	msg_set_handler(kMsgExternPowerStatusChange, app_on_extern_power_changed);
    msg_set_handler(kMsgBatteryStatusWhenMotorRun, app_on_battery_level_changed_when_motor_run);
	//msg_set_handler(kMsgBatteryStatusChange, app_on_battery_status_changed);
	msg_set_handler(kMsgTimeChanged, app_on_time_changed);
	msg_set_handler(kMsgInstallStateChanged, app_on_install_state_changed);
	msg_set_handler(kMsgCloseDisplay, app_on_close_display);
	msg_set_handler(kMsgGoToSleep, app_on_go_to_sleep);
	msg_set_handler(kMsgAlarmVoiceMute, app_on_alarm_voice_mute);
	msg_set_handler(kMsgAlarmVoiceUnMute, app_on_alarm_voice_unmute);
}
