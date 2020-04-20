/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-20 xqzhao
* Initial revision.
*
************************************************/
#include "scr_precision_calibration.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_lable.h"
#include "ui_number_edit.h"
#include "data.h"
#include "common.h"

#include "infusion_motor.h"
#include "app_popup.h"
#include "mid_common.h"
#include "param.h"
#include "record_log.h"

/************************************************
* Declaration
************************************************/
#define TOTAL_VOLUME_INDICATOR_X 32
#define TOTAL_VOLUME_INDICATOR_Y FIRST_LINE_Y
#define TOTAL_VOLUME_INDICATOR_WIDTH 64
#define TOTAL_VOLUME_INDICATOR_HEIGHT ONE_LINE_HEIGHT

#define TOTAL_VOLUME_X (TOTAL_VOLUME_INDICATOR_X+TOTAL_VOLUME_INDICATOR_WIDTH)
#define TOTAL_VOLUME_Y TOTAL_VOLUME_INDICATOR_Y
#define TOTAL_VOLUME_WIDTH TOTAL_VOLUME_INDICATOR_WIDTH
#define TOTAL_VOLUME_HEIGHT TOTAL_VOLUME_INDICATOR_HEIGHT

#define TOTAL_VOLUME_UNIT_X (TOTAL_VOLUME_X+TOTAL_VOLUME_WIDTH)
#define TOTAL_VOLUME_UNIT_Y TOTAL_VOLUME_INDICATOR_Y
#define TOTAL_VOLUME_UNIT_WIDTH TOTAL_VOLUME_INDICATOR_WIDTH
#define TOTAL_VOLUME_UNIT_HEIGHT TOTAL_VOLUME_INDICATOR_HEIGHT

#define REAL_VOLUME_INDICATOR_X TOTAL_VOLUME_INDICATOR_X
#define REAL_VOLUME_INDICATOR_Y (FIRST_LINE_Y+ONE_LINE_HEIGHT)
#define REAL_VOLUME_INDICATOR_WIDTH TOTAL_VOLUME_INDICATOR_WIDTH
#define REAL_VOLUME_INDICATOR_HEIGHT ONE_LINE_HEIGHT

#define REAL_VOLUME_X (REAL_VOLUME_INDICATOR_X+REAL_VOLUME_INDICATOR_WIDTH)
#define REAL_VOLUME_Y REAL_VOLUME_INDICATOR_Y
#define REAL_VOLUME_WIDTH REAL_VOLUME_INDICATOR_WIDTH
#define REAL_VOLUME_HEIGHT REAL_VOLUME_INDICATOR_HEIGHT

#define REAL_VOLUME_UNIT_X (REAL_VOLUME_X+REAL_VOLUME_WIDTH)
#define REAL_VOLUME_UNIT_Y REAL_VOLUME_INDICATOR_Y
#define REAL_VOLUME_UNIT_WIDTH REAL_VOLUME_INDICATOR_WIDTH
#define REAL_VOLUME_UNIT_HEIGHT REAL_VOLUME_INDICATOR_HEIGHT

#define PRECISION_CALIBRATION_UPDATE_VOLUME_TIME_MS 100

#define PRECISION_CALIBRATION_RUN_PERIOD_TIMER_ID kTimerScreen2
#define PRECISION_CALIBRATION_RUN_PERIOD_TIME_MS 5000

void scr_precision_calibration_start_run();
static void scr_precision_calibration_update();

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_precision_calibration_widgets[] = {
	kWidgetAttrBase,kUILable, kUI_IDLable1, 
		TOTAL_VOLUME_INDICATOR_X,TOTAL_VOLUME_INDICATOR_Y,TOTAL_VOLUME_INDICATOR_WIDTH,TOTAL_VOLUME_INDICATOR_HEIGHT,
		kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,kWidgetAttrTextID,kStrInfusionDose,
	kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit1,
		TOTAL_VOLUME_X,TOTAL_VOLUME_Y,TOTAL_VOLUME_WIDTH,TOTAL_VOLUME_HEIGHT,
		kWidgetAttrAlign,UI_ALIGN_RIGHT | UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable3, 
        TOTAL_VOLUME_UNIT_X,TOTAL_VOLUME_UNIT_Y,TOTAL_VOLUME_UNIT_WIDTH,TOTAL_VOLUME_UNIT_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,        

    kWidgetAttrBase,kUILable, kUI_IDLable4, 
        REAL_VOLUME_INDICATOR_X,REAL_VOLUME_INDICATOR_Y,REAL_VOLUME_INDICATOR_WIDTH,REAL_VOLUME_INDICATOR_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,kWidgetAttrTextID,kStrRealInfusionDose,
    kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit2,
        REAL_VOLUME_X,REAL_VOLUME_Y,REAL_VOLUME_WIDTH,REAL_VOLUME_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_RIGHT | UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable6, 
        REAL_VOLUME_UNIT_X,REAL_VOLUME_UNIT_Y,REAL_VOLUME_UNIT_WIDTH,REAL_VOLUME_UNIT_HEIGHT,
        kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,          
};

static int g_precision_calibration_run_encoder;
static bool g_precision_calibration_is_running;
/************************************************
* Function 
************************************************/
void scr_precision_on_motor_stop(int run_encoder, int stop_cause) {
	g_precision_calibration_run_encoder += run_encoder;
	if (screen_current_id() == kScreenPrecisionCalibration) {
		//UINumberEdit* edit;
		//edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit1));
		//edit->set_value(encoder_to_dose(g_precision_calibration_run_encoder));

		//edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit2));
		//edit->set_value(encoder_to_dose(g_precision_calibration_run_encoder));
		if (stop_cause == MOTOR_STOP_BY_FINISH_EXPECT_TARGET)
		{
			screen_set_timer_handler(PRECISION_CALIBRATION_RUN_PERIOD_TIMER_ID, scr_precision_calibration_start_run);
			timer_start_oneshot_after(PRECISION_CALIBRATION_RUN_PERIOD_TIMER_ID,
				PRECISION_CALIBRATION_RUN_PERIOD_TIME_MS);
		}
		scr_precision_calibration_update();
	}
}
void scr_precision_calibration_start_run() {
	g_precision_calibration_is_running = true;
	infusion_motor_start_ex(kForward, -1, scr_precision_on_motor_stop);
}
void scr_precision_calibration_stop_run() {
	g_precision_calibration_is_running = false;
	infusion_motor_stop(0);
	timer_stop(PRECISION_CALIBRATION_RUN_PERIOD_TIMER_ID);
}
/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static char g_buff[30];
static void scr_precision_calibration_on_press_ok_key(KeyID key, int repeat_count)
{
    LogVar log_vars[2];
    
    if (g_precision_calibration_run_encoder>0&&!g_precision_calibration_is_running) {
		UINumberEdit* edit;
		edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit2));
		int encoder_count_of_one_ml = (int)(g_precision_calibration_run_encoder*1000.0/edit->value()) ;
		data_write_int(kDataEncoderCountEachML, encoder_count_of_one_ml);
		set_encoder_count_of_one_ml(encoder_count_of_one_ml * data_read_int(kDataPrecisionFactor) / 100.0);
		Screen* curr = screen_current();
        snprintf(g_buff, sizeof(g_buff), "Success! encoder/mL: %d", encoder_count_of_one_ml);
		app_popup_enter(kPopupInformation, NULL, g_buff, NULL);
		screen_delete(curr);

        log_vars[0].type = kLogVarInteger;
        log_vars[0].value = encoder_count_of_one_ml;
		log_vars[1].type = kLogVarInteger;
		log_vars[1].value = edit->value();
        record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
            kLogEventPrecisionCalibrationDone,log_vars, ssz_array_size(log_vars));
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
static void scr_precision_calibration_on_press_left_key(KeyID key, int repeat_count)
{
	UIView* p;
	UINumberEdit* q;
	UIMsg msg;

    msg.msg_id = kUIMsgKeyPress;

    p = ui_view_focused();
    if (p) {
        msg.key_info_param.key = UI_KEY_DECREASE;
    }
	q = (UINumberEdit*)p;
	if (repeat_count >= 70) {
		q->init(1, 0xffffff, 1, 5000);
	}
	else if (repeat_count >= 50) {
		q->init(1, 0xffffff, 1, 1000);
	}	
	else if (repeat_count >= 30) {
		q->init(1, 0xffffff, 1, 100);
	}
	else if (repeat_count >= 10) {
		q->init(1, 0xffffff, 1, 10);
	}
	else {
		q->init(1, 0xffffff, 1, 1);
	}
	ui_view_send_msg(p, msg);

}

/***********************************************
* Description:
*   handle ok key
* Argument:
*
* Return:
*
************************************************/
static void scr_precision_calibration_on_press_right_key(KeyID key, int repeat_count)
{
	UIView* p;
    UINumberEdit* q;
	UIMsg msg;

    msg.msg_id = kUIMsgKeyPress;
    p = ui_view_focused();
    if (p) {
        msg.key_info_param.key = UI_KEY_INCREASE;
    }

    q=(UINumberEdit*)p;
	
	if (repeat_count >= 70) {
		q->init(1, 0xffffff, 1, 5000);
	}
	else if (repeat_count >= 50) {
		q->init(1, 0xffffff, 1, 1000);
	}	
	else if(repeat_count >= 30){
        q->init(1,0xffffff,1,100);
    }
	else if (repeat_count >= 10) {
		q->init(1, 0xffffff, 1, 10);
	}
	else{
        q->init(1,0xffffff,1,1);
    }
	ui_view_send_msg(p, msg);

}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void scr_precision_calibration_on_press_back_key(KeyID key, int repeat_count)
{
	scr_precision_calibration_stop_run();
    screen_go_back();
}


static void scr_precision_calibration_on_press_start_key(KeyID key, int repeat_count)
{
	if (g_precision_calibration_is_running) {
		scr_precision_calibration_stop_run();

		UINumberEdit* edit;

		edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit2));
		edit->set_value(encoder_to_dose(g_precision_calibration_run_encoder));

		scr_precision_calibration_update();
	} else {
		
		g_precision_calibration_run_encoder = 0;
		UINumberEdit* edit;

		edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit2));
		edit->set_value(encoder_to_dose(g_precision_calibration_run_encoder));
		scr_precision_calibration_update();

		scr_precision_calibration_start_run();
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
static void scr_precision_calibration_on_scr_create(Screen* scr)
{
	g_precision_calibration_run_encoder = 0;
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
static void scr_precision_calibration_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    scr_layout_load_standard(scr, get_string(kStrInfusionPrecisionCalibration), get_string(kStrReturn), get_string(kStrSave));
	UILable* P;

	P = M_ui_lable(scr_view->get_child(kUI_IDLable3));
	P->set_text("uL");
	P = M_ui_lable(scr_view->get_child(kUI_IDLable6));
	P->set_text("uL");
	UINumberEdit* edit;
	edit = M_ui_number_edit(scr_view->get_child(kUI_IDNumberEdit1));
	edit->init(0, 100000, 1, 1);
	edit = M_ui_number_edit(scr_view->get_child(kUI_IDNumberEdit2));
	edit->init(0, 100000, 1, 1);
}

static void scr_precision_calibration_update() {


	UINumberEdit* edit;
	edit = M_ui_number_edit(screen_current_view()->get_child(kUI_IDNumberEdit1));
	edit->set_value(encoder_to_dose(g_precision_calibration_run_encoder));
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_precision_calibration_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, scr_precision_calibration_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, scr_precision_calibration_on_press_back_key);
	key_set_handler(kKeyStart, kKeyEventPressed, scr_precision_calibration_on_press_start_key);
	
	key_set_handler_with_repeat(kKeyLeft, kKeyEventPressed,  scr_precision_calibration_on_press_left_key);
	key_set_handler_with_repeat(kKeyRight, kKeyEventPressed,  scr_precision_calibration_on_press_right_key);


	//screen_set_timer_handler(kTimerScreen, scr_precision_calibration_update);
	//scr_precision_calibration_update();
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
static void scr_precision_calibration_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_precision_calibration_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_precision_calibration_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_precision_calibration_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
			if (g_precision_calibration_is_running) {
				scr_precision_calibration_stop_run();
			}
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
void scr_precision_calibration_enter(void)
{   
    screen_enter(kScreenPrecisionCalibration, scr_precision_calibration_callback,NULL,
        g_precision_calibration_widgets, ssz_array_size(g_precision_calibration_widgets));
}
