/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-15 xqzhao
* Initial revision.
*
************************************************/
#include "scr_factory_mode.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_text_list.h"
#include "ui_list_pos_indicator.h"
#include "version.h"
#include "app_popup.h"
#include "scr_pressure_calibration.h"
#include "scr_precision_calibration.h"
#include "app_scr_change_password.h"
#include "data.h"
#include "app_mcu_comm.h"
#include "app_scr_change_voice_volume.h"
#include "app_scr_change_oled_brightness.h"
#include "app_operation_log.h"
#include "record_log_config.h"
#include "record_log.h"
#include "scr_battery_calibration.h"
#include "app_alternative_popup.h"
#include "param.h"
#include "scr_new_pressure_calibration.h"
#include "scr_pressure_threshold.h"
#include "scr_precision_factor.h"


#ifdef SSZ_TARGET_MACHINE
#include "core_cm3.h"
#endif
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_factory_mode_widgets[] = {
  kWidgetAttrNull
};

static const StrID g_factory_mode_items[] = {
	kStrPressureCalibration,
	kStrInfusionPrecisionCalibration,
	kStrBatteryCalibration,
	kStrSystemInfo,
	kStrDefaultValue,
 	kStrChangePassword,
	kStrChangeOledBrightness,
	kStrBubbleAlarmSwitch,
    kStrBlockThresholdSet,
	kStrPrecesionFactorSet,
	kStrOperationLog,
};
/************************************************
* Function 
************************************************/
void scr_factory_mode_on_select_bubble_alarm_setting(PopupReturnCode ret_code, int select_index) {
    LogVar log_var;
    log_var.type = kLogVarString;
    bool bubble_alarm;

    if (ret_code==kPopupReturnOK) {
        
        if (select_index == 1) {
            bubble_alarm = true;
            log_var.str_value = "on";
        } else {
            bubble_alarm = false;
            log_var.str_value = "off";
        }
        
        set_bubble_alarm_status(bubble_alarm);
        data_write_int(kDataBubbleAlarmSwitch, bubble_alarm);
        record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
                       kLogEventSetBubbleAlarm, &log_var, 1);
    }
}

/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void scr_factory_mode_on_press_ok_key(KeyID key, int repeat_count)
{
	UITextList* p = M_ui_text_list(screen_current_view()->get_child(kUI_IDTextList));
	switch (g_factory_mode_items[p->get_selected()]) {
	case kStrPressureCalibration:
		scr_new_pressure_calibration_enter();
		break;
	case kStrInfusionPrecisionCalibration:
		scr_precision_calibration_enter();
		break;
	case kStrBatteryCalibration:
		scr_battery_calibration_enter();
		break;
	case kStrSystemInfo: 
	{
		app_mcu_send_to_slave(COMM_ASK_SLAVER_MCU_VERSION,0,0);
		char sn[SERIAL_NUMBER_LEN + 1] = {0};
		data_read(kDataSerialNumber, sn, SERIAL_NUMBER_LEN);
		char *tmp = get_dynamic_string(kStrDynamic1);
		if(app_mcu_receive_slaver_version_ture()){
			snprintf(tmp, DYNAMIC_STRING_MAX_SIZE, "Master V%d.%d.%d.%d, Slaver V%d.%d.%d.%d\nS/N:%s",
			version_info()->major, version_info()->minor,version_info()->revision, version_info()->internal_version,
			slaver_version_info()->major, slaver_version_info()->minor,slaver_version_info()->revision, slaver_version_info()->internal_version,sn);
		} else{
			snprintf(tmp, DYNAMIC_STRING_MAX_SIZE, "Master V%d.%d.%d.%d, Slaver No Respond\nS/N:%s",
			version_info()->major, version_info()->minor,version_info()->revision, version_info()->internal_version, sn);
		}

		app_popup_enter(kPopupInformation, NULL, tmp, NULL);
	}
		break;
	case kStrChangePassword:
		app_scr_change_password_enter();
		break;
	case kStrDefaultValue:
	{
		data_erase_block(kDataUserBlock);	
		record_log_clear(kHistoryLog);
		data_erase_block(kDataUserInitBlock);
		record_log_add_info_with_data(kOperationLog, kLogEventSystemReset, NULL, 0);
		app_popup_enter(kPopupRemind, NULL, "Factory Data Reset Success", NULL);
		app_mcu_send_to_slave(COMM_SLAVER_MCU_STOP_DOG,0,0 );  // 停止看门狗
		#ifdef SSZ_TARGET_MACHINE
				NVIC_SystemReset();
		#endif
	}		
		break;	
	case kStrChangeOledBrightness:
		app_scr_change_oled_brightness_enter();
		break;
	case kStrOperationLog:
		app_operation_log_enter();
		break;
	case kStrBubbleAlarmSwitch: 
	{
		bool is_enable_bubble_alarm = data_read_int(kDataBubbleAlarmSwitch);
		app_alternative_popup_enter(get_string(kStrBubbleAlarmSwitch), get_string(kStrOff), get_string(kStrOn), is_enable_bubble_alarm, scr_factory_mode_on_select_bubble_alarm_setting);
		break;
	}
	case kStrBlockThresholdSet:
		scr_pressure_threshold_enter();
		break;
	case kStrPrecesionFactorSet:
		scr_precision_factor_enter();
		break;
    
	default:
		ssz_assert_fail();
		break;
	}
}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void scr_factory_mode_on_press_back_key(KeyID key, int repeat_count)
{
	exit_factory_mode();
}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void scr_factory_mode_on_scr_create(Screen* scr)
{

}

/***********************************************
* Description:
*   handle screen widget init, at this you can add widget, 
*   set widget's attribute(like color, font, add item to list)
* Argument:
*
* Return:
*
************************************************/
static void scr_factory_mode_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    scr_layout_load_one_column_list(scr, get_string(kStrFactoryMode), get_string(kStrReturn), get_string(kStrEnter));
	UITextList* p = M_ui_text_list(scr_view->get_child(kUI_IDTextList));

	for (int i = 0; i < ssz_array_size(g_factory_mode_items); i++) {
		p->push_back(get_string(g_factory_mode_items[i]));
	}
	UIListPosIndicator* p2 = M_ui_list_pos_indicator(scr->view->get_child(kUI_IDListPosIndicator));
	p2->set_max_value(p->size());
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_factory_mode_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, scr_factory_mode_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, scr_factory_mode_on_press_back_key);

	UITextList* p = (UITextList*)scr->view->get_child(kUI_IDTextList);
	UIListPosIndicator* p2 = M_ui_list_pos_indicator(scr->view->get_child(kUI_IDListPosIndicator));
	p2->set_value(p->get_selected() + 1);
}

/***********************************************
* Description:
*   msg handler
* Argument:
*   msg:
*
* Return:
*
************************************************/
static void scr_factory_mode_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_factory_mode_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_factory_mode_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_factory_mode_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
		case kUIMsgNotifyParent:
			if (msg->int_param == kUINotifySelectChanged &&
				msg->src->id() == kUI_IDTextList) {
				UITextList* p = (UITextList*)scr->view->get_child(kUI_IDTextList);
				UIListPosIndicator* p2 = M_ui_list_pos_indicator(scr->view->get_child(kUI_IDListPosIndicator));
				p2->set_value(p->get_selected() + 1);
			}				
			break;
        default:
            break;
    }
    screen_on_msg_default(scr, msg);
}

/***********************************************
* Description:
*   enter the screen
* Argument:
*
* Return:
*
************************************************/
void scr_factory_mode_enter(void)
{   
    screen_enter(kScreenFactoryMode, scr_factory_mode_callback,NULL,
        g_factory_mode_widgets, ssz_array_size(g_factory_mode_widgets));	
	app_mcu_send_to_slave(COMM_ASK_SLAVER_MCU_VERSION,0,0);	
}
