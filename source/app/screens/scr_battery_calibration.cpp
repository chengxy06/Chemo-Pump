/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2018-07-31 xqzhao
* Initial revision.
*
************************************************/
#include "scr_battery_calibration.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_lable.h"
#include "common.h"
#include "data.h"
#include "record_log.h"
#include "app_popup.h"
#include "param.h"

/************************************************
* Declaration
************************************************/
#define CURRENT_VALUE_INDICATOR_X 32
#define CURRENT_VALUE_INDICATOR_Y FIRST_LINE_Y
#define CURRENT_VALUE_INDICATOR_WIDTH 64
#define CURRENT_VALUE_INDICATOR_HEIGHT ONE_LINE_HEIGHT

#define CURRENT_VALUE_X (CURRENT_VALUE_INDICATOR_X+CURRENT_VALUE_INDICATOR_WIDTH)
#define CURRENT_VALUE_Y CURRENT_VALUE_INDICATOR_Y
#define CURRENT_VALUE_WIDTH CURRENT_VALUE_INDICATOR_WIDTH
#define CURRENT_VALUE_HEIGHT CURRENT_VALUE_INDICATOR_HEIGHT

#define CURRENT_VALUE_UNIT_X (CURRENT_VALUE_X+CURRENT_VALUE_WIDTH)
#define CURRENT_VALUE_UNIT_Y CURRENT_VALUE_INDICATOR_Y
#define CURRENT_VALUE_UNIT_WIDTH CURRENT_VALUE_INDICATOR_WIDTH
#define CURRENT_VALUE_UNIT_HEIGHT CURRENT_VALUE_INDICATOR_HEIGHT

#define SAVED_VALUE_INDICATOR_X CURRENT_VALUE_INDICATOR_X
#define SAVED_VALUE_INDICATOR_Y (FIRST_LINE_Y+ONE_LINE_HEIGHT)
#define SAVED_VALUE_INDICATOR_WIDTH CURRENT_VALUE_INDICATOR_WIDTH
#define SAVED_VALUE_INDICATOR_HEIGHT ONE_LINE_HEIGHT

#define SAVED_VALUE_X (SAVED_VALUE_INDICATOR_X+SAVED_VALUE_INDICATOR_WIDTH)
#define SAVED_VALUE_Y SAVED_VALUE_INDICATOR_Y
#define SAVED_VALUE_WIDTH SAVED_VALUE_INDICATOR_WIDTH
#define SAVED_VALUE_HEIGHT SAVED_VALUE_INDICATOR_HEIGHT

#define SAVED_VALUE_UNIT_X (SAVED_VALUE_X+SAVED_VALUE_WIDTH)
#define SAVED_VALUE_UNIT_Y SAVED_VALUE_INDICATOR_Y
#define SAVED_VALUE_UNIT_WIDTH SAVED_VALUE_INDICATOR_WIDTH
#define SAVED_VALUE_UNIT_HEIGHT SAVED_VALUE_INDICATOR_HEIGHT

#define BATTERY_CALIBRATION_UPDATE_ADC_TIME_MS 20
/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_battery_calibration_widgets[] = {
	kWidgetAttrBase,kUILable, kUI_IDLable1,
	CURRENT_VALUE_INDICATOR_X,CURRENT_VALUE_INDICATOR_Y,CURRENT_VALUE_INDICATOR_WIDTH,CURRENT_VALUE_INDICATOR_HEIGHT,
	kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,kWidgetAttrTextID,kStrCurrentOffsetValue,
	kWidgetAttrBase,kUILable, kUI_IDLable2,
	CURRENT_VALUE_X,CURRENT_VALUE_Y,CURRENT_VALUE_WIDTH,CURRENT_VALUE_HEIGHT,
	kWidgetAttrAlign,UI_ALIGN_RIGHT | UI_ALIGN_VCENTER,
	kWidgetAttrBase,kUILable, kUI_IDLable3,
	CURRENT_VALUE_UNIT_X,CURRENT_VALUE_UNIT_Y,CURRENT_VALUE_UNIT_WIDTH,CURRENT_VALUE_UNIT_HEIGHT,
	kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,

	kWidgetAttrBase,kUILable, kUI_IDLable4,
	SAVED_VALUE_INDICATOR_X,SAVED_VALUE_INDICATOR_Y,SAVED_VALUE_INDICATOR_WIDTH,SAVED_VALUE_INDICATOR_HEIGHT,
	kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,kWidgetAttrTextID,kStrSavedOffsetValue,
	kWidgetAttrBase,kUILable, kUI_IDLable5,
	SAVED_VALUE_X,SAVED_VALUE_Y,SAVED_VALUE_WIDTH,SAVED_VALUE_HEIGHT,
	kWidgetAttrAlign,UI_ALIGN_RIGHT | UI_ALIGN_VCENTER,
	kWidgetAttrBase,kUILable, kUI_IDLable6,
	SAVED_VALUE_UNIT_X,SAVED_VALUE_UNIT_Y,SAVED_VALUE_UNIT_WIDTH,SAVED_VALUE_UNIT_HEIGHT,
	kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,
};

static int g_battery_calibration_last_show_value;
/************************************************
* Function 
************************************************/

/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void scr_battery_calibration_on_press_ok_key(KeyID key, int repeat_count)
{
	int ad_diff = g_battery_calibration_last_show_value;
    LogVar log_vars[1];

	if (ad_diff < 500 && ad_diff>-500) {
		data_write_int(kDataBatteryRedress, ad_diff);
		set_battery_offset_ad(ad_diff);
		Screen* curr = screen_current();
		app_popup_enter(kPopupInformation, NULL, "Success", NULL);
		//screen_delete(curr);

        log_vars[0].type = kLogVarInteger;
        log_vars[0].value = ad_diff;
        record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
			kLogEventBatteryCalibrationDone,log_vars, ssz_array_size(log_vars));
	}
	else {
		app_popup_enter(kPopupInformation, NULL, "Fail", NULL);
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
static void scr_battery_calibration_on_press_back_key(KeyID key, int repeat_count)
{
    screen_go_back();
}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void scr_battery_calibration_on_scr_create(Screen* scr)
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
static void scr_battery_calibration_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	scr_layout_load_standard(scr, get_string(kStrBatteryCalibration), get_string(kStrReturn), get_string(kStrSave));
	UILable* P;

	P = M_ui_lable(scr_view->get_child(kUI_IDLable3));
	P->set_text(" AD");
	P = M_ui_lable(scr_view->get_child(kUI_IDLable6));
	P->set_text(" AD");
}

static void scr_battery_calibration_update() {


	int value = mid_adc_get_average_ADC_channel_value(kSysPwrADC)- M_get_adc_from_battery_voltage(3000);
	if (value != g_battery_calibration_last_show_value) {
		g_battery_calibration_last_show_value = value;
		snprintf(get_dynamic_string(kStrDynamic2), DYNAMIC_STRING_MAX_SIZE, "%d", value);

		UILable* P;
		P = M_ui_lable(screen_current_view()->get_child(kUI_IDLable2));
		P->set_text_and_force_invalid(get_dynamic_string(kStrDynamic2));
	}

}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_battery_calibration_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, scr_battery_calibration_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, scr_battery_calibration_on_press_back_key);

	int ad = data_read_int(kDataBatteryRedress);
	snprintf(get_dynamic_string(kStrDynamic1), DYNAMIC_STRING_MAX_SIZE, "%d", ad);
	UILable* P;

	P = M_ui_lable(scr_view->get_child(kUI_IDLable5));
	P->set_text_and_force_invalid(get_dynamic_string(kStrDynamic1));

	//set show as invalid
	g_battery_calibration_last_show_value = ADC_MAX_DIGIT_CODE*2;
	screen_set_timer_handler(kTimerScreen, scr_battery_calibration_update);
	timer_start_periodic_every(kTimerScreen, BATTERY_CALIBRATION_UPDATE_ADC_TIME_MS);
	scr_battery_calibration_update();
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
static void scr_battery_calibration_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_battery_calibration_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_battery_calibration_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_battery_calibration_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
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
void scr_battery_calibration_enter(void)
{   
    screen_enter(kScreenBatteryCalibration, scr_battery_calibration_callback,NULL,
        g_battery_calibration_widgets, ssz_array_size(g_battery_calibration_widgets));
}
