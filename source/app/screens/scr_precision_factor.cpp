/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2019-03-26 jlli
* Initial revision.
*
************************************************/
#include "scr_precision_factor.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_number_edit.h"
#include "ui_lable.h"
#include "data.h"
#include "param.h"
#include "record_log.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_precision_factor_widgets[] = {
    kWidgetAttrBase, kUINumberEdit, kUI_IDNumberEdit, 80, 4, 105, 42,
    kWidgetAttrFontHeight, LARGE_FONT_HEIGHT,
};

/************************************************
* Function 
************************************************/

/***********************************************
* Description:
*   handle left key
* Argument:
*
* Return:
*
************************************************/
static void scr_precision_factor_on_press_left_key(KeyID key, int repeat_count)
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
    if (repeat_count >= 10) {
        q->init(MIN_PRECISION_FACTOR, MAX_PRECISION_FACTOR, 100, 10);
    }else{
        q->init(MIN_PRECISION_FACTOR, MAX_PRECISION_FACTOR, 100, 1);
    }
    ui_view_send_msg(p, msg);
}

/***********************************************
* Description:
*   handle right key
* Argument:
*
* Return:
*
************************************************/
static void scr_precision_factor_on_press_right_key(KeyID key, int repeat_count)
{
    UIView* p;
    UINumberEdit* q;
    UIMsg msg;

    msg.msg_id = kUIMsgKeyPress;
    p = ui_view_focused();
    if (p) {
        msg.key_info_param.key = UI_KEY_INCREASE;
    }

    q = (UINumberEdit*)p;
    if (repeat_count >= 10) {
        q->init(MIN_PRECISION_FACTOR, MAX_PRECISION_FACTOR, 100, 10);
    }else{
        q->init(MIN_PRECISION_FACTOR, MAX_PRECISION_FACTOR, 100, 1);
    }

    ui_view_send_msg(p, msg);
}

/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void scr_precision_factor_on_press_ok_key(KeyID key, int repeat_count)
{
    UINumberEdit* p = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit);
    LogVar log_var;

    data_write_int(kDataPrecisionFactor, p->value());
    set_encoder_count_of_one_ml(data_read_int(kDataEncoderCountEachML) * p->value() / 100.0);
    log_var.type = kLogVarFloatWithOneDecimal;
    log_var.value = p->value();
    record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
        kLogEventPrecisionFactor, &log_var, 1);
    screen_go_back();

}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void scr_precision_factor_on_press_back_key(KeyID key, int repeat_count)
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
static void scr_precision_factor_on_scr_create(Screen* scr)
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
static void scr_precision_factor_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    UINumberEdit* p = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit);
    int factor;

    scr_layout_load_standard(scr, get_string(kStrPrecesionFactorSet), get_string(kStrReturn), get_string(kStrConfirm));

    factor = data_read_int(kDataPrecisionFactor);

    p->set_value(factor);
	p->init(MIN_PRECISION_FACTOR, MAX_PRECISION_FACTOR, 100, 1);
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_precision_factor_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, scr_precision_factor_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, scr_precision_factor_on_press_back_key);

    key_set_handler_with_repeat(kKeyLeft, kKeyEventPressed, scr_precision_factor_on_press_left_key);
    key_set_handler_with_repeat(kKeyRight, kKeyEventPressed, scr_precision_factor_on_press_right_key);
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
static void scr_precision_factor_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_precision_factor_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_precision_factor_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_precision_factor_on_scr_activate(scr, scr->view);
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
void scr_precision_factor_enter(void)
{   
    screen_enter(kScreenPrecisionFactor, scr_precision_factor_callback, NULL,
        g_precision_factor_widgets, ssz_array_size(g_precision_factor_widgets));
}
