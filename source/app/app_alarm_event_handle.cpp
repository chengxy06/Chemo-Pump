#include "app_alarm_event_handle.h"
#include "msg.h"
#include "alarm.h"
#include "infusion_motor.h"
#include "three_valve_motor.h"
#include "app_delivery.h"
#include "screen.h"
#include "app_scr_alarm.h"
#include "app_main_menu.h"
#include "alarm_play.h"
#include "record_log.h"
#include "param.h"
#include "app_system_monitor.h"
#include "mid_common.h"
#include "led.h"
#include "app_alternative_popup.h"
#include "data.h"
#include "app_common.h"

/************************************************
* Declaration
************************************************/
#define PLAY_BEEP_INTERVALS_MS 500
#define PLAY_BEEP_EXPECT_TIME  3

/************************************************
* Variable
************************************************/
static AlarmID g_app_last_highest_alarm = kAlarmNull;
/************************************************
* Function
************************************************/
static void app_check_last_infuse_confirm_popup_callback(PopupReturnCode ret_code)
{
    if (ret_code == kPopupReturnOK) {
        power_off();
    }
}

static void app_check_last_infuse_on_clear_user_data_finished(PopupReturnCode ret_code)
{
    switch(ret_code){
        case kPopupReturnOK:
        {
            app_popup_enter(kPopupInformation, get_string(kStrPowerOffConfirm),
                get_string(kStrIsConfirm), app_check_last_infuse_confirm_popup_callback);
        }
            break;

        case kPopupReturnCancel:

            break;
    }
}

static void app_check_last_infuse_on_clear_user_data(PopupReturnCode ret_code)
{
	switch(ret_code){
		case kPopupReturnOK:
		{
            alarm_clear(kAlarmUserParamReadErrorID);
            data_erase_block(kDataUserInitBlock);
            app_popup_enter(kPopupInformation, NULL, get_string(kStrUserDataCleared), app_check_last_infuse_on_clear_user_data_finished);;
		}
			break;

		case kPopupReturnCancel:

			break;
	}
}
void app_alarm_play(AlarmID alarm_id)
{
	AlarmLevel alarm_level = alarm_id_to_level(alarm_id);

	if (alarm_level == kAlarmLevelNone) {
		alarm_stop_play();
	}
	else if (alarm_level >= kAlarmLevelHigh) {
		alarm_play(kAlarmPlayHighAlarm);
	}
	else if (alarm_level >= kAlarmLevelLow) {
		alarm_play(kAlarmPlayLowAlarm);
	}
	else if (alarm_level >= kAlarmLevelLowest) {
		alarm_play(kAlarmPlayLowAlarm);
	}
	else {
		if (alarm_id == kAlarmInfusionNearEmptyID ||
			alarm_id == kAlarmAdapterDisconnectedID) {
			alarm_play_beep_several_times(PLAY_BEEP_EXPECT_TIME, PLAY_BEEP_INTERVALS_MS);
		}
	}
}

void app_alarm_update_voice_and_display() {
	//if highest alarm changed, need update display and voice
	if (alarm_highest_priority_id() != g_app_last_highest_alarm) {
		g_app_last_highest_alarm = alarm_highest_priority_id();
		screen_delete_by_id(kScreenAlarm);
		alarm_stop_play();
		alarm_stop_play_beep_several_times();

        if(g_app_last_highest_alarm == kAlarmUserParamReadErrorID ){
             //alarm_clear(kAlarmUserParamReadErrorID);
            app_alarm_play(g_app_last_highest_alarm);
            app_popup_enter(kPopupWarning, NULL, get_string(kStrUserDataError), app_check_last_infuse_on_clear_user_data);

		} else if (g_app_last_highest_alarm!= kAlarmNull) {
			//play
            app_alarm_play(g_app_last_highest_alarm);
			//display
			app_scr_alarm_enter(g_app_last_highest_alarm);
		}
		//如果没有报警,亮起绿灯
		if(is_use_extern_power() && alarm_highest_priority_level()<kAlarmLevelLowest){
			led_turn_on(kLedGreen);
		}	
	}
}

void app_alarm_auto_close_remind() {
	alarm_clear(kAlarmInfusionNearEmptyID);
	alarm_clear(kAlarmAdapterDisconnectedID);
}
//handle new alarm
void app_alarm_on_set(MsgParam param)
{
	AlarmID alarm_id = (AlarmID)param.int_param;
	AlarmLevel alarm_level = alarm_id_to_level(alarm_id);
	
	LogVar log_var;
	
	//reset check no operate
	app_system_monitor_set_to_default_no_operate_time();

	if (alarm_level>=kAlarmLevelLowest&&alarm_level<=kAlarmLevelHigh) {
		log_var.type = kLogVarAlarmID;
		log_var.value = alarm_id;
		record_log_add(kHistoryLog, LOG_EVENT_TYPE_ALARM, kLogEventAlarm,
			&log_var, 1);
	}
	else if (alarm_level == kAlarmLevelHighest) {
		log_var.type = kLogVarInteger;
		log_var.value = alarm_id - kHighestAlarmStart;
		record_log_add(kHistoryLog, LOG_EVENT_TYPE_ERROR, kLogEventSystemError,
			&log_var,1);
	}

	app_alarm_update_voice_and_display();
	if (alarm_level>=kAlarmLevelHigh) {
		if (infusion_motor_is_running()) {
			infusion_motor_stop(MOTOR_STOP_CUSTOM_CAUSE_START+alarm_id);
		}
		if (three_valve_motor_is_running()) {
			three_valve_motor_stop(MOTOR_STOP_CUSTOM_CAUSE_START + alarm_id);
		}
		InfusionType curr_run_type =  app_delivery_curr_running_infusion();
		if (curr_run_type!=kInfusionTypeMax) {
			app_delivery_pause(curr_run_type);
		}
	}

	if (alarm_id == kAlarmInfusionNearEmptyID ||
		alarm_id == kAlarmAdapterDisconnectedID) {
		timer_set_handler(kTimerCloseRemind, app_alarm_auto_close_remind);
		timer_start_oneshot_after(kTimerCloseRemind, REMIND_SHOW_TIME_MS);
	}
}

//handle clear alam 
void app_alarm_on_clear(MsgParam param)
{
	int alarm_id = param.int_param;

	app_alarm_update_voice_and_display();
	if (alarm_id == kAlarmInfusionOverID && g_app_last_highest_alarm== kAlarmNull) {
		//if no alarm exist, and infusion is over, go to main menu
		if (screen_is_exist(kScreenMainMenu)) {
			screen_go_back_to_id(kScreenMainMenu);
		}
		else {
			app_main_menu_enter();
		}
	}
}
void app_alarm_event_handle_init()
{
	//msg_set_handler(kMsgAlarmSet, app_alarm_on_set);
	//msg_set_handler(kMsgAlarmClear, app_alarm_on_clear);
}
