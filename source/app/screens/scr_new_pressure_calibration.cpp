/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2019-01-2 xqzhao
* Initial revision.
*
************************************************/
#include "scr_new_pressure_calibration.h"
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
#include "ui_number_edit.h"
#include "alarm.h"

/************************************************
* Declaration
************************************************/
#define CURRENT_VALUE_INDICATOR_X 0
#define CURRENT_VALUE_INDICATOR_Y FIRST_LINE_Y
#define CURRENT_VALUE_INDICATOR_WIDTH 64
#define CURRENT_VALUE_INDICATOR_HEIGHT ONE_LINE_HEIGHT

#define CURRENT_VALUE_X (CURRENT_VALUE_INDICATOR_X+CURRENT_VALUE_INDICATOR_WIDTH)
#define CURRENT_VALUE_Y CURRENT_VALUE_INDICATOR_Y
#define CURRENT_VALUE_WIDTH 32
#define CURRENT_VALUE_HEIGHT CURRENT_VALUE_INDICATOR_HEIGHT

#define CURRENT_VALUE_UNIT_X (CURRENT_VALUE_X+CURRENT_VALUE_WIDTH)
#define CURRENT_VALUE_UNIT_Y CURRENT_VALUE_INDICATOR_Y
#define CURRENT_VALUE_UNIT_WIDTH CURRENT_VALUE_INDICATOR_WIDTH
#define CURRENT_VALUE_UNIT_HEIGHT CURRENT_VALUE_INDICATOR_HEIGHT

#define CURRENT_PRESSURE_INDICATOR_X CURRENT_VALUE_INDICATOR_X
#define CURRENT_PRESSURE_INDICATOR_Y (FIRST_LINE_Y+ONE_LINE_HEIGHT)
#define CURRENT_PRESSURE_INDICATOR_WIDTH CURRENT_VALUE_INDICATOR_WIDTH
#define CURRENT_PRESSURE_INDICATOR_HEIGHT ONE_LINE_HEIGHT

#define CURRENT_PRESSURE_X (CURRENT_PRESSURE_INDICATOR_X+CURRENT_PRESSURE_INDICATOR_WIDTH)
#define CURRENT_PRESSURE_Y CURRENT_PRESSURE_INDICATOR_Y
#define CURRENT_PRESSURE_WIDTH CURRENT_VALUE_WIDTH
#define CURRENT_PRESSURE_HEIGHT CURRENT_PRESSURE_INDICATOR_HEIGHT

#define CURRENT_PRESSURE_UNIT_X (CURRENT_PRESSURE_X+CURRENT_PRESSURE_WIDTH)
#define CURRENT_PRESSURE_UNIT_Y CURRENT_PRESSURE_INDICATOR_Y
#define CURRENT_PRESSURE_UNIT_WIDTH CURRENT_PRESSURE_INDICATOR_WIDTH
#define CURRENT_PRESSURE_UNIT_HEIGHT CURRENT_PRESSURE_INDICATOR_HEIGHT

#define CURRENT_STEP_INDICATOR_X 128
#define CURRENT_STEP_INDICATOR_Y FIRST_LINE_Y
#define CURRENT_STEP_INDICATOR_WIDTH 64
#define CURRENT_STEP_INDICATOR_HEIGHT ONE_LINE_HEIGHT

#define CURRENT_STEP_X (CURRENT_STEP_INDICATOR_X+CURRENT_STEP_INDICATOR_WIDTH)
#define CURRENT_STEP_Y CURRENT_STEP_INDICATOR_Y
#define CURRENT_STEP_WIDTH CURRENT_STEP_INDICATOR_WIDTH
#define CURRENT_STEP_HEIGHT CURRENT_STEP_INDICATOR_HEIGHT

#define RUN_STATE_ICON_X ((SCREEN_WINDOW_WIDTH-16)/2)
#define RUN_STATE_ICON_Y ((NAVIGATION_BAR_Y-16)/2)

#define PRESSURE_CALIBRATION_UPDATE_ADC_TIME_MS 20

#define PRESSURE_CALIBRATION_RUN_ONCE_TIMER_ID kTimerScreen1
#define PRESSURE_CALIBRATION_RUN_ONCE_TIME_MS 100
#define PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID kTimerScreen2
#define PRESSURE_CALIBRATION_RUN_PERIOD_TIME_MS 5000
#define PRESSURE_CALIBRATION_GET_ADC_TIMER_ID kTimerScreen3
#define PRESSURE_CALIBRATION_GET_ADC_TIME_MS 2000

#define PRESSURE_CALIBRATION_MAX_STEP 10

void scr_new_pressure_calibration_start_run();

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_new_pressure_calibration_widgets[] = {
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
        CURRENT_PRESSURE_INDICATOR_X,CURRENT_PRESSURE_INDICATOR_Y,CURRENT_PRESSURE_INDICATOR_WIDTH,CURRENT_PRESSURE_INDICATOR_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,kWidgetAttrTextID,kStrCurrentPressure,
    kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit1, 
        CURRENT_PRESSURE_X,CURRENT_PRESSURE_Y,CURRENT_PRESSURE_WIDTH,CURRENT_PRESSURE_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_RIGHT | UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable6, 
        CURRENT_PRESSURE_UNIT_X,CURRENT_PRESSURE_UNIT_Y,CURRENT_PRESSURE_UNIT_WIDTH,CURRENT_PRESSURE_UNIT_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,          


	kWidgetAttrBase,kUILable, kUI_IDLable7, 
		CURRENT_STEP_INDICATOR_X,CURRENT_STEP_INDICATOR_Y,CURRENT_STEP_INDICATOR_WIDTH,CURRENT_STEP_INDICATOR_HEIGHT,
		kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,kWidgetAttrTextID,kStrCurrentStep,
	kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit2, 
		CURRENT_STEP_X,CURRENT_STEP_Y,CURRENT_STEP_WIDTH,CURRENT_STEP_HEIGHT,
		kWidgetAttrAlign,UI_ALIGN_RIGHT | UI_ALIGN_VCENTER,


	kWidgetAttrBase,kUIImage, kUI_IDImage,
		RUN_STATE_ICON_X,RUN_STATE_ICON_Y,0,0,
};

static int g_new_pressure_calibration_last_show_voltage;
static bool g_new_pressure_calibration_is_running;
//static bool g_new_pressure_pressure_error;


static int g_new_pressure_calibration_current_step;
static int g_new_pressure_calibration_last_adc_after_motor_stop;
static int g_new_pressure_calibration_last_adc_after_motor_stop_diff;

static int g_new_pressure_calibration_adcs[PRESSURE_CALIBRATION_MAX_STEP];
static int g_new_pressure_calibration_adc_diffs[PRESSURE_CALIBRATION_MAX_STEP];
static int g_new_pressure_calibration_pressures[PRESSURE_CALIBRATION_MAX_STEP];
/************************************************
* Function 
************************************************/

static void scr_new_pressure_calibration_auto_update() {

	int voltage = M_get_voltage_from_adc(pressure_bubble_sensor_get_pressure_sensor_ADC_average_value());
	if (voltage != g_new_pressure_calibration_last_show_voltage) {
		g_new_pressure_calibration_last_show_voltage = voltage;
		snprintf(get_dynamic_string(kStrDynamic2), DYNAMIC_STRING_MAX_SIZE, "%d", voltage);

		UILable* P;
		P = M_ui_lable(screen_current_view()->get_child(kUI_IDLable2));
		P->set_text_and_force_invalid(get_dynamic_string(kStrDynamic2));
	}

}

void scr_new_pressure_calibration_update_run_state() {
	UIImage* p_img = M_ui_image(screen_current_view()->get_child(kUI_IDImage));
	if (g_new_pressure_calibration_is_running) {
		p_img->set_img(get_image(kImgInfusionRun));
	} else {
		p_img->set_img(get_image(kImgInfusionPause));
	}
}

void scr_new_pressure_calibration_update() {
	scr_new_pressure_calibration_auto_update();
	scr_new_pressure_calibration_update_run_state();

	UINumberEdit* edit;

	edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit2));
	edit->set_value(g_new_pressure_calibration_current_step+1);
}

void scr_new_pressure_calibration_stop_motor() {
	infusion_motor_stop(0);
}
void scr_new_pressure_calibration_get_adc_time_out() {
	int tmp = pressure_bubble_sensor_get_pressure_sensor_ADC_average_value();
	g_new_pressure_calibration_last_adc_after_motor_stop_diff = tmp - g_new_pressure_calibration_last_adc_after_motor_stop;
	g_new_pressure_calibration_last_adc_after_motor_stop = tmp;
	
	ssz_traceln("get adc[%d], adc_diff[%d]", g_new_pressure_calibration_last_adc_after_motor_stop, 
		g_new_pressure_calibration_last_adc_after_motor_stop_diff);
}
void scr_new_pressure_calibration_on_motor_stop(int run_encoder, int stop_cause){
	g_new_pressure_calibration_is_running = false;
	if (screen_current_id() == kScreenNewPressureCalibration) {
		screen_set_timer_handler(PRESSURE_CALIBRATION_GET_ADC_TIMER_ID, scr_new_pressure_calibration_get_adc_time_out);
		timer_start_oneshot_after(PRESSURE_CALIBRATION_GET_ADC_TIMER_ID,
			PRESSURE_CALIBRATION_GET_ADC_TIME_MS);
		scr_new_pressure_calibration_update();

//        if (g_new_pressure_calibration_current_step == 0 && g_new_pressure_calibration_last_show_voltage > 500) {
//            app_popup_enter(kPopupWarning, NULL, "pressure sensor error!!!", NULL);
//            g_new_pressure_pressure_error = true;
//        }
        
//        if (g_new_pressure_pressure_error) {
//            app_popup_enter(kPopupWarning, NULL, "pressure sensor error!!!", NULL);
//        }
	}

}

void scr_new_pressure_calibration_start_run() {
	g_new_pressure_calibration_is_running = true;
	infusion_motor_start_ex(kForward, -1, scr_new_pressure_calibration_on_motor_stop);

//	screen_set_timer_handler(PRESSURE_CALIBRATION_RUN_ONCE_TIMER_ID, scr_new_pressure_calibration_stop_motor);
//	timer_start_oneshot_after(PRESSURE_CALIBRATION_RUN_ONCE_TIMER_ID,
//		PRESSURE_CALIBRATION_RUN_ONCE_TIME_MS);
//
//	screen_set_timer_handler(PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID, scr_new_pressure_calibration_start_run);
//	timer_start_oneshot_after(PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID,
//		PRESSURE_CALIBRATION_RUN_PERIOD_TIME_MS);

	scr_new_pressure_calibration_update_run_state();
}
void scr_new_pressure_calibration_stop_run() {
	g_new_pressure_calibration_is_running = false;
	infusion_motor_stop(0);
	timer_stop(PRESSURE_CALIBRATION_RUN_ONCE_TIMER_ID);
	timer_stop(PRESSURE_CALIBRATION_RUN_PERIOD_TIMER_ID);
	scr_new_pressure_calibration_update_run_state();
}

/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static char g_buff[50];
static void scr_new_pressure_calibration_on_press_ok_key(KeyID key, int repeat_count)
{
	UINumberEdit* edit;
	edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit1));
	
	int pressure = edit->value();
	if (pressure>0) {
		g_new_pressure_calibration_pressures[g_new_pressure_calibration_current_step] = pressure;
		g_new_pressure_calibration_adcs[g_new_pressure_calibration_current_step] = 
			g_new_pressure_calibration_last_adc_after_motor_stop;
		g_new_pressure_calibration_adc_diffs[g_new_pressure_calibration_current_step] = 
			g_new_pressure_calibration_last_adc_after_motor_stop_diff;
		g_new_pressure_calibration_current_step++;

		if (g_new_pressure_calibration_current_step>=PRESSURE_CALIBRATION_MAX_STEP ||
			(pressure>=75&&g_new_pressure_calibration_current_step>=2)) {
			common_printfln("pressure calibration data:");
			for (int i = 0; i < g_new_pressure_calibration_current_step; i++) {
				common_printfln("%d: adc[%d],pressure[%d]",i, g_new_pressure_calibration_adcs[i],
					g_new_pressure_calibration_pressures[i]);
			}
			LogVar log_vars[3];
			//calc the slope
			LineEquationPara para;
			para = fit_line_by_data(g_new_pressure_calibration_adcs,
				g_new_pressure_calibration_pressures,
				g_new_pressure_calibration_current_step);
			common_printfln("Slope: %d", para.a);
			if(para.a>0){
				data_write_int(kDataOcclusionSlope, para.a);

				//calc one drop increase adc
				int sum = 0;
				common_printfln("pressure calibration adc diff:");
				for (int i = 1; i < g_new_pressure_calibration_current_step; i++) {
					sum += g_new_pressure_calibration_adc_diffs[i];
					common_printfln("%d: one drop adc[%d]",i, g_new_pressure_calibration_adc_diffs[i]);
				}
				ssz_check(g_new_pressure_calibration_current_step>1);
				//int average = sum / (g_new_pressure_calibration_current_step-1);
				int average = (g_new_pressure_calibration_adc_diffs[g_new_pressure_calibration_current_step - 1]
				              + g_new_pressure_calibration_adc_diffs[g_new_pressure_calibration_current_step - 2])
				              / 2;
				data_write_int(kDataOcclusionIncreaseOfOneDrop, average);
				common_printfln("one drop adc average[%d]",average);

				log_vars[0].type = kLogVarFloat;
				log_vars[0].value = para.a;
				log_vars[1].type = kLogVarInteger;
				log_vars[1].value = average;
				log_vars[2].type = kLogVarInteger;
				log_vars[2].value = g_new_pressure_calibration_current_step;
				record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO,
					kLogEventNewPressureCalibrationDone, log_vars, ssz_array_size(log_vars));
				ssz_traceln("pressure calibration success, slope[%d],one drop adc[%d], step[%d]",
					para.a, average, g_new_pressure_calibration_current_step);
				Screen* curr = screen_current();
                snprintf(g_buff, sizeof(g_buff), "Success!\nslope:%d, adc/drop:%d", para.a, average);
				app_popup_enter(kPopupInformation, NULL, g_buff, NULL);
				screen_delete(curr);
			}else{
				g_new_pressure_calibration_current_step = 0;
				app_popup_enter(kPopupInformation, NULL, "Fail", NULL);
			}
		} else {
			UINumberEdit* edit;

			edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit1));
			edit->set_value(0);
			scr_new_pressure_calibration_update();
		}
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
static void scr_new_pressure_calibration_on_press_back_key(KeyID key, int repeat_count)
{
	scr_new_pressure_calibration_stop_run();
    screen_go_back();
}

static void scr_new_pressure_calibration_on_press_start_key(KeyID key, int repeat_count)
{
	if (g_new_pressure_calibration_is_running) {
		scr_new_pressure_calibration_stop_run();
	} else {
		//g_new_pressure_calibration_pressure_adc_when_start =
		//	pressure_bubble_sensor_get_pressure_sensor_ADC_average_value();
		scr_new_pressure_calibration_start_run();
	}
}
/***********************************************
* Description:
*   handle left key
* Argument:
*
* Return:
*
************************************************/
static void scr_new_pressure_calibration_on_press_left_key(KeyID key, int repeat_count)
{
	UINumberEdit* q;
	UIMsg msg;

	msg.msg_id = kUIMsgKeyPress;
	msg.key_info_param.key = UI_KEY_DECREASE;

	q=M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit1));;

	if (repeat_count > 0) {
		q->init(0, 100, 1, 10);
	}
	else{
		q->init(0,100,1,1);
	}
	ui_view_send_msg(q, msg);

}

/***********************************************
* Description:
*   handle ok key
* Argument:
*
* Return:
*
************************************************/
static void scr_new_pressure_calibration_on_press_right_key(KeyID key, int repeat_count)
{
	UINumberEdit* q;
	UIMsg msg;

	msg.msg_id = kUIMsgKeyPress;
	msg.key_info_param.key = UI_KEY_INCREASE;

	q=M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit1));;

	 if (repeat_count > 0) {
		q->init(0, 100, 1, 10);
	}
	else{
		q->init(0,100,1,1);
	}
	ui_view_send_msg(q, msg);

}
/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void scr_new_pressure_calibration_on_scr_create(Screen* scr)
{
	pressure_and_bubble_sensor_pwr_enable();
	g_new_pressure_calibration_current_step = 0;
	ssz_mem_zero(g_new_pressure_calibration_adcs, sizeof(g_new_pressure_calibration_adcs));
	ssz_mem_zero(g_new_pressure_calibration_adc_diffs, sizeof(g_new_pressure_calibration_adc_diffs));
	ssz_mem_zero(g_new_pressure_calibration_pressures, sizeof(g_new_pressure_calibration_pressures));
	g_new_pressure_calibration_last_adc_after_motor_stop = 0;
	g_new_pressure_calibration_last_adc_after_motor_stop_diff = 0;
    //g_new_pressure_pressure_error = false;
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
static void scr_new_pressure_calibration_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    scr_layout_load_standard(scr, get_string(kStrPressureCalibration), get_string(kStrReturn), get_string(kStrSave));
	UILable* P;

	P = M_ui_lable(scr_view->get_child(kUI_IDLable3));
	P->set_text("mV");
	P = M_ui_lable(scr_view->get_child(kUI_IDLable6));
	P->set_text("kPa");

	UIImage* p_img = M_ui_image(screen_current_view()->get_child(kUI_IDImage));
	p_img->set_auto_size(true);

	UINumberEdit* edit;
	edit = M_ui_number_edit(scr_view->get_child(kUI_IDNumberEdit1));
	edit->init(0, 100, 1, 1);

	edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit2));
	edit->init(1,10,1,1);

	g_new_pressure_calibration_is_running = false;
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_new_pressure_calibration_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, scr_new_pressure_calibration_on_press_ok_key);
	key_set_handler(kKeyBack, kKeyEventPressed, scr_new_pressure_calibration_on_press_back_key);
	key_set_handler(kKeyStart, kKeyEventPressed, scr_new_pressure_calibration_on_press_start_key);

	key_set_handler_with_repeat(kKeyLeft, kKeyEventPressed,  scr_new_pressure_calibration_on_press_left_key);
	key_set_handler_with_repeat(kKeyRight, kKeyEventPressed,  scr_new_pressure_calibration_on_press_right_key);
	//int ad = data_read_int(kDataOcclusionThreshold);
	//snprintf(get_dynamic_string(kStrDynamic1), DYNAMIC_STRING_MAX_SIZE, "%d", M_get_voltage_from_adc(ad));
	//UILable* P;

	//P = M_ui_lable(scr_view->get_child(kUI_IDLable5));
	//P->set_text_and_force_invalid(get_dynamic_string(kStrDynamic1));


	//set show as invalid
	g_new_pressure_calibration_last_show_voltage = -1;
	screen_set_timer_handler(kTimerScreen, scr_new_pressure_calibration_auto_update);
	timer_start_periodic_every(kTimerScreen, PRESSURE_CALIBRATION_UPDATE_ADC_TIME_MS);
	scr_new_pressure_calibration_update();
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
static void scr_new_pressure_calibration_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_new_pressure_calibration_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_new_pressure_calibration_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_new_pressure_calibration_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
			if (g_new_pressure_calibration_is_running) {
				scr_new_pressure_calibration_stop_run();
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
void scr_new_pressure_calibration_enter(void)
{   
    screen_enter(kScreenNewPressureCalibration, scr_new_pressure_calibration_callback,NULL,
        g_new_pressure_calibration_widgets, ssz_array_size(g_new_pressure_calibration_widgets));
}
