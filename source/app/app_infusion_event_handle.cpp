/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-28 xqzhao
* Initial revision.
*
************************************************/
#include "app_infusion_event_handle.h"
#include "msg.h"
#include "ssz_common.h"
#include "app_infusion_monitor.h"
#include "screen.h"
#include "app_delivery.h"
#include "app_popup.h"
#include "alarm.h"
#include "screen.h"
#include "ui_define.h"
#include "record_log.h"
#include "app_sleep.h"
#include "param.h"
#include "string_data.h"
#include "app_common.h"
#include "data.h"
#include "mid_common.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
static void app_infusion_pause_too_long() {
	
	if (system_mode() == kSystemModeFactory) {
		//when at factory mode, no active this alarm
        return;
    }
	
	alarm_set(kAlarmInfusionPauseTooLongID);
	app_sleep_exit();
}
void app_infusion_add_start_log(InfusionType infusion_type) {

	if (infusion_type == kNormalInfusion) {
		InfusionInfo* info = app_delivery_info(infusion_type);
		LogVar log_var[3];
		log_var[0].type = kLogVarFloatWithOneDecimal;
		log_var[0].value = info->infusion_speed / 100;
		log_var[1].type = kLogVarInteger;
		log_var[1].value = info->total_dose / 1000;
		log_var[2].type = kLogVarInteger;
		log_var[2].value = info->meds_b_total_dose / 1000;
		record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
			kLogEventInfusionStart, log_var, ssz_array_size(log_var));
		if (info->is_peek_infusion_mode) {
			log_var[0].type = kLogVarInteger;
			log_var[0].value = info->peek_infusion_start_time_point;
			log_var[1].type = kLogVarInteger;
			log_var[1].value = info->peek_infusion_end_time_point;
			log_var[2].type = kLogVarFloatWithOneDecimal;
			log_var[2].value = info->peek_infusion_speed / 100;
			record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
				kLogEventInfusionPeekSet, log_var, ssz_array_size(log_var));
		}
	}
	else if (infusion_type == kCleanTube) {
		InfusionInfo* info = app_delivery_info(infusion_type);
		LogVar log_var[1];
		log_var[0].type = kLogVarInteger;
		log_var[0].value = info->meds_b_total_dose/1000;
		record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
			kLogEventCleanTubeStart, log_var, ssz_array_size(log_var));
	}
	else if (infusion_type == kClearAir) {
		record_log_add_info_with_data(kHistoryLog, kLogEventClearAirStart, NULL, 0);
	}
}
void app_infusion_add_pause_log(InfusionType infusion_type) {

	if (infusion_type == kNormalInfusion) {
		InfusionInfo* info = app_delivery_info(infusion_type);
		LogVar log_var[2];
		log_var[0].type = kLogVarFloat;
		log_var[0].value = info->infused_dose;
		log_var[1].type = kLogVarFloat;
		log_var[1].value = info->meds_b_infused_dose;
		record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
			kLogEventInfusionPause, log_var, ssz_array_size(log_var));
	}
	else if (infusion_type == kCleanTube) {
		InfusionInfo* info = app_delivery_info(infusion_type);
		LogVar log_var[1];
		log_var[0].type = kLogVarFloat;
		log_var[0].value = info->meds_b_infused_dose;
		record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
			kLogEventCleanTubePause, log_var, ssz_array_size(log_var));
	}
	else if (infusion_type == kClearAir) {
		InfusionInfo* info = app_delivery_info(infusion_type);
		LogVar log_var[2];
		log_var[0].type = kLogVarFloat;
		log_var[0].value = info->infused_dose;
		log_var[1].type = kLogVarFloat;
		log_var[1].value = info->meds_b_infused_dose;
		record_log_add_info(kHistoryLog, kLogEventClearAirStop, 
			log_var, ssz_array_size(log_var));
	}
}
void app_infusion_add_resume_log(InfusionType infusion_type) {

	if (infusion_type == kNormalInfusion) {
		record_log_add_with_data(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
			kLogEventInfusionResume, NULL, 0);
	}
	else if (infusion_type == kCleanTube) {
		record_log_add_with_data(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
			kLogEventCleanTubeResume, NULL, 0);
	}
	else if (infusion_type == kClearAir) {
		record_log_add_info_with_data(kHistoryLog, kLogEventClearAirStart, NULL, 0);
	}
}
void app_infusion_add_stop_log(InfusionType infusion_type) {

	if (infusion_type == kNormalInfusion) {
		InfusionInfo* info = app_delivery_info(infusion_type);
		LogVar log_var[2];
		log_var[0].type = kLogVarFloat;
		log_var[0].value = info->infused_dose;
		log_var[1].type = kLogVarFloat;
		log_var[1].value = info->meds_b_infused_dose;
		record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
			kLogEventInfusionStop, log_var, ssz_array_size(log_var));
	}
	else if (infusion_type == kCleanTube) {
		InfusionInfo* info = app_delivery_info(infusion_type);
		LogVar log_var[1];
		log_var[0].type = kLogVarFloat;
		log_var[0].value = info->meds_b_infused_dose;
		record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
			kLogEventCleanTubeStop, log_var, ssz_array_size(log_var));
	}
	else if (infusion_type == kClearAir) {
		InfusionInfo* info = app_delivery_info(infusion_type);
		LogVar log_var[2];
		log_var[0].type = kLogVarFloat;
		log_var[0].value = info->infused_dose;
		log_var[1].type = kLogVarFloat;
		log_var[1].value = info->meds_b_infused_dose;
		record_log_add_info(kHistoryLog, kLogEventClearAirStop,
			log_var, ssz_array_size(log_var));
	}
}
void app_infusion_on_run_internal(InfusionType infusion_type) {
	if (infusion_type == kClearAir) {
		app_infusion_monitor_start(INFUSION_MONITOR_OCCLUSION_FLAG |
			INFUSION_MONITOR_INSTALL_NOT_IN_PLACE_FLAG);
	}
	else if (infusion_type == kCleanTube) {
		app_infusion_monitor_start(INFUSION_MONITOR_OCCLUSION_FLAG |
			INFUSION_MONITOR_BUBBLE_FLAG |
			INFUSION_MONITOR_INSTALL_NOT_IN_PLACE_FLAG);
	}
	else {
		app_infusion_monitor_start(INFUSION_MONITOR_ALL_FLAG);
	}
	timer_stop(kTimerNoOperateWhenPause);
	screen_send_msg_no_para(screen_current(), kUIMsgDeliveryStateChanged, NULL);
}
void app_infusion_on_pause_internal(InfusionType infusion_type) {
	app_infusion_monitor_stop();

    if (alarm_highest_priority_level() < kAlarmLevelHigh)
	    timer_start_oneshot_after(kTimerNoOperateWhenPause, NO_OPERATE_TOO_LONG_MS_WHEN_INFUSION_PAUSE);

	screen_send_msg_no_para(screen_current(), kUIMsgDeliveryStateChanged, NULL);
}
void app_infusion_on_stop_internal(InfusionType infusion_type) {
	app_infusion_monitor_stop();
	timer_stop(kTimerNoOperateWhenPause);
	screen_send_msg_no_para(screen_current(), kUIMsgDeliveryStateChanged, NULL);
}
void app_infusion_on_start(MsgParam param)
{
	InfusionType infusion_type = (InfusionType)param.int16_param1;
	if (param.int16_param2 == DELIVERY_START_TO_RUN) {//start to run
		app_infusion_on_run_internal(infusion_type);
		app_infusion_add_start_log(infusion_type);
	}
	else {
		//start to pause
		app_infusion_on_pause_internal(infusion_type);
		app_infusion_add_pause_log(infusion_type);
	}
}
#if TEST_LIFETIME_ENABLE
void app_infusion_next_to_test_lifetime(MsgParam param) {
	app_delivery_start_with_param(kNormalInfusion, &g_test_lifetime_infusion_param);
	screen_go_back_to_home();
}
void app_infusion_next_to_test_lifetime_time_out() {
	app_sleep_exit();
	alarm_clear(kAlarmInfusionOverID);
	msg_set_handler(kMsgInfusionNextToTestLifeTime, app_infusion_next_to_test_lifetime);
	msg_post_int(kMsgInfusionNextToTestLifeTime, 0);
	
}
#endif
void app_infusion_on_pause(MsgParam param)
{
	InfusionType infusion_type = (InfusionType)param.int16_param1;
	app_infusion_on_pause_internal(infusion_type);

	if (param.int16_param2 == DELIVERY_PAUSE_BY_FINISH_DELIVERY) {
		app_delivery_stop(infusion_type);
		if (infusion_type == kNormalInfusion) {
			alarm_set(kAlarmInfusionOverID);

#if TEST_LIFETIME_ENABLE
			if (g_is_testing_lifetime) {
				low_power_timer_set_handler(kTimerInfusionNextToTestLifetime, app_infusion_next_to_test_lifetime_time_out);
				timer_start_oneshot_after(kTimerInfusionNextToTestLifetime, 1000);
				int lifetime_count = data_read_int(kDataRunLifetimeCount);
				lifetime_count++;
				data_write_int(kDataRunLifetimeCount, lifetime_count);
			}
#endif // TEST_LIFETIME_ENABLE
		}
		else if (infusion_type == kClearAir) {
			app_popup_enter(kPopupInformation, NULL, get_string(kStrClearAirDone), NULL);
			screen_delete_by_id(kScreenScrAutoClearAir);
		}
		else {
			app_popup_enter(kPopupInformation, NULL, get_string(kStrCleanTubeDone), NULL);
			screen_delete_by_id(kScreenCleanTube);
		}
	}
	else{
		//pause by user or alarm, need add log
		app_infusion_add_pause_log(infusion_type);
	}
}

void app_infusion_on_resume(MsgParam param)
{
	InfusionType infusion_type = (InfusionType)param.int16_param1;
	app_infusion_on_run_internal(infusion_type);
	app_infusion_add_resume_log(infusion_type);
}

void app_infusion_on_stop(MsgParam param)
{
	InfusionType infusion_type = (InfusionType)param.int16_param1;
	app_infusion_on_stop_internal(infusion_type);
	if (infusion_type==kClearAir && param.int16_param2==DELIVERY_STOP_FROM_PAUSE) {
		//no need add log
	}
	else {
		app_infusion_add_stop_log(infusion_type);
	}
}

void app_infusion_on_one_point_stop(MsgParam param)
{
	set_sleep_allow(true);
}

//void app_infusion_on_change_to_b_bag(MsgParam param)
//{
//	record_log_add_with_data(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
//		kLogEventChangeToSmallBagToInfusion, NULL, 0);
//}

void app_infusion_event_handle_init()
{
	msg_set_handler(kMsgInfusionStart, app_infusion_on_start);
	msg_set_handler(kMsgInfusionResume, app_infusion_on_resume);
	msg_set_handler(kMsgInfusionPause, app_infusion_on_pause);
	msg_set_handler(kMsgInfusionStop, app_infusion_on_stop);
	msg_set_handler(kMsgInfuseOnePointStop, app_infusion_on_one_point_stop);
	//msg_set_handler(kMsgInfusionChangeToBagBMeds, app_infusion_on_change_to_b_bag);


	low_power_timer_set_handler(kTimerNoOperateWhenPause, app_infusion_pause_too_long);
}
