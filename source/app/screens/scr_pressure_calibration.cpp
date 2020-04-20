/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-18 xqzhao
* Initial revision.
*
************************************************/
#include "scr_pressure_calibration.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_lable.h"
#include "data.h"
#include "common.h"
#include "pressure_bubble_sensor.h"
#include "infusion_motor.h"
#include "app_popup.h"
#include "image_data.h"
#include "ui_image.h"
#include "record_log.h"

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

#define RUN_STATE_ICON_X ((SCREEN_WINDOW_WIDTH-16)/2)
#define RUN_STATE_ICON_Y ((NAVIGATION_BAR_Y-16)/2)

#define PRESSURE_CALIBRATION_UPDATE_ADC_TIME_MS 20

#define PRESSURE_CALIBRATION_RUN_ONCE_TIMER_ID kTimerScreen1
#define PRESSURE_CALIBRATION_RUN_ONCE_TIME_MS 100
#define PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID kTimerScreen2
#define PRESSURE_CALIBRATION_RUN_PERIOD_TIME_MS 5000

void scr_pressure_calibration_start_run();

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_pressure_calibration_widgets[] = {
	kWidgetAttrBase,kUILable, kUI_IDLable1, 
		CURRENT_VALUE_INDICATOR_X,CURRENT_VALUE_INDICATOR_Y,CURRENT_VALUE_INDICATOR_WIDTH,CURRENT_VALUE_INDICATOR_HEIGHT,
		kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,kWidgetAttrTextID,kStrCurrentValue,
	kWidgetAttrBase,kUILable, kUI_IDLable2, 
		CURRENT_VALUE_X,CURRENT_VALUE_Y,CURRENT_VALUE_WIDTH,CURRENT_VALUE_HEIGHT,
		kWidgetAttrAlign,UI_ALIGN_RIGHT | UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable3, 
        CURRENT_VALUE_UNIT_X,CURRENT_VALUE_UNIT_Y,CURRENT_VALUE_UNIT_WIDTH,CURRENT_VALUE_UNIT_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,        

    kWidgetAttrBase,kUILable, kUI_IDLable4, 
        SAVED_VALUE_INDICATOR_X,SAVED_VALUE_INDICATOR_Y,SAVED_VALUE_INDICATOR_WIDTH,SAVED_VALUE_INDICATOR_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,kWidgetAttrTextID,kStrSavedValue,
    kWidgetAttrBase,kUILable, kUI_IDLable5, 
        SAVED_VALUE_X,SAVED_VALUE_Y,SAVED_VALUE_WIDTH,SAVED_VALUE_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_RIGHT | UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable6, 
        SAVED_VALUE_UNIT_X,SAVED_VALUE_UNIT_Y,SAVED_VALUE_UNIT_WIDTH,SAVED_VALUE_UNIT_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,          

	kWidgetAttrBase,kUIImage, kUI_IDImage,
		RUN_STATE_ICON_X,RUN_STATE_ICON_Y,0,0,
};

static int g_pressure_calibration_last_show_voltage;
static bool g_pressure_calibration_is_running;
static int g_pressure_calibration_pressure_adc_when_start;
/************************************************
* Function 
************************************************/

void scr_pressure_calibration_update_run_state() {
	UIImage* p_img = M_ui_image(screen_current_view()->get_child(kUI_IDImage));
	if (g_pressure_calibration_is_running) {
		p_img->set_img(get_image(kImgInfusionRun));
	} else {
		p_img->set_img(get_image(kImgInfusionPause));
	}
}
void scr_pressure_calibration_stop_motor() {
	infusion_motor_stop(0);
}
void scr_pressure_calibration_on_motor_stop(int run_encoder, int stop_cause){
	if (screen_current_id() == kScreenPressureCalibration) {
		screen_set_timer_handler(PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID, scr_pressure_calibration_start_run);
		timer_start_oneshot_after(PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID,
			PRESSURE_CALIBRATION_RUN_PERIOD_TIME_MS);
		scr_pressure_calibration_update_run_state();
	}

}

void scr_pressure_calibration_start_run() {
	g_pressure_calibration_is_running = true;
	infusion_motor_start_ex(kForward, -1, scr_pressure_calibration_on_motor_stop);

//	screen_set_timer_handler(PRESSURE_CALIBRATION_RUN_ONCE_TIMER_ID, scr_pressure_calibration_stop_motor);
//	timer_start_oneshot_after(PRESSURE_CALIBRATION_RUN_ONCE_TIMER_ID,
//		PRESSURE_CALIBRATION_RUN_ONCE_TIME_MS);
//
//	screen_set_timer_handler(PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID, scr_pressure_calibration_start_run);
//	timer_start_oneshot_after(PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID,
//		PRESSURE_CALIBRATION_RUN_PERIOD_TIME_MS);

	scr_pressure_calibration_update_run_state();
}
void scr_pressure_calibration_stop_run() {
	g_pressure_calibration_is_running = false;
	infusion_motor_stop(0);
	timer_stop(PRESSURE_CALIBRATION_RUN_ONCE_TIMER_ID);
	timer_stop(PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID);
	scr_pressure_calibration_update_run_state();
}

/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void scr_pressure_calibration_on_press_ok_key(KeyID key, int repeat_count)
{
	int ad_diff = pressure_bubble_sensor_get_pressure_sensor_ADC_average_value()-
		g_pressure_calibration_pressure_adc_when_start;
    LogVar log_vars[2];

	scr_pressure_calibration_stop_run();
	if (ad_diff > 10) {
		data_write_int(kDataOcclusionThreshold, ad_diff);
		Screen* curr = screen_current();
		app_popup_enter(kPopupInformation, NULL, "Success", NULL);
		screen_delete(curr);

        log_vars[0].type = kLogVarInteger;
        log_vars[0].value = ad_diff;
		log_vars[1].type = kLogVarInteger;
		log_vars[1].value = g_pressure_calibration_pressure_adc_when_start;
        record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
            kLogEventPressureCalibrationDone,log_vars, ssz_array_size(log_vars));
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
static void scr_pressure_calibration_on_press_back_key(KeyID key, int repeat_count)
{
	scr_pressure_calibration_stop_run();
    screen_go_back();
}

static void scr_pressure_calibration_on_press_start_key(KeyID key, int repeat_count)
{
	if (g_pressure_calibration_is_running) {
		scr_pressure_calibration_stop_run();
	} else {
		g_pressure_calibration_pressure_adc_when_start =
			pressure_bubble_sensor_get_pressure_sensor_ADC_average_value();
		scr_pressure_calibration_start_run();
	}
}
/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void scr_pressure_calibration_on_scr_create(Screen* scr)
{
	pressure_and_bubble_sensor_pwr_enable();
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
static void scr_pressure_calibration_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    scr_layout_load_standard(scr, get_string(kStrPressureCalibration), get_string(kStrReturn), get_string(kStrSave));
	UILable* P;

	P = M_ui_lable(scr_view->get_child(kUI_IDLable3));
	P->set_text("mV");
	P = M_ui_lable(scr_view->get_child(kUI_IDLable6));
	P->set_text("mV");

	UIImage* p_img = M_ui_image(screen_current_view()->get_child(kUI_IDImage));
	p_img->set_auto_size(true);

	g_pressure_calibration_is_running = false;
}

static void scr_pressure_calibration_update() {


	int voltage = M_get_voltage_from_adc(pressure_bubble_sensor_get_pressure_sensor_ADC_average_value());
	if (voltage != g_pressure_calibration_last_show_voltage) {
		g_pressure_calibration_last_show_voltage = voltage;
		snprintf(get_dynamic_string(kStrDynamic2), DYNAMIC_STRING_MAX_SIZE, "%d", voltage);

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
static void scr_pressure_calibration_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, scr_pressure_calibration_on_press_ok_key);
	key_set_handler(kKeyBack, kKeyEventPressed, scr_pressure_calibration_on_press_back_key);
	key_set_handler(kKeyStart, kKeyEventPressed, scr_pressure_calibration_on_press_start_key);

	int ad = data_read_int(kDataOcclusionThreshold);
	snprintf(get_dynamic_string(kStrDynamic1), DYNAMIC_STRING_MAX_SIZE, "%d", M_get_voltage_from_adc(ad));
	UILable* P;

	P = M_ui_lable(scr_view->get_child(kUI_IDLable5));
	P->set_text_and_force_invalid(get_dynamic_string(kStrDynamic1));

	//set show as invalid
	g_pressure_calibration_last_show_voltage = -1;
	screen_set_timer_handler(kTimerScreen, scr_pressure_calibration_update);
	timer_start_periodic_every(kTimerScreen, PRESSURE_CALIBRATION_UPDATE_ADC_TIME_MS);
	scr_pressure_calibration_update();

	scr_pressure_calibration_update_run_state();
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
static void scr_pressure_calibration_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_pressure_calibration_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_pressure_calibration_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_pressure_calibration_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
			if (g_pressure_calibration_is_running) {
				scr_pressure_calibration_stop_run();
			}
            break;
        case kUIMsgScrDestroy:
			pressure_and_bubble_sensor_pwr_disable();
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
void scr_pressure_calibration_enter(void)
{   
    screen_enter(kScreenPressureCalibration, scr_pressure_calibration_callback,NULL,
        g_pressure_calibration_widgets, ssz_array_size(g_pressure_calibration_widgets));
}
