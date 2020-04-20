/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-29 jlli
* Initial revision.
*
************************************************/
#include "scr_set_clean_tube_dose.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "ui_number_edit.h"
#include "scr_layout.h"
#include "app_scr_set_infusion_speed.h"
#include "scr_password_popup.h"
#include "app_alternative_popup.h"
#include "data.h"
#include "app_scr_set_infusion_duration.h"
#include "app_set_menu.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_set_clean_tube_dose_widgets[] = {
    kWidgetAttrBase,kUILable, kUI_IDLable, 175,36,14,10,
    kWidgetAttrTextID,kStrMililiter,
    kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit, 83,4,90,42,
    kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,
};

int static g_max_clean_tube_dose;
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
void scr_set_clean_on_select_infusion_mode_select(PopupReturnCode ret_code, int select_index) {
    if (ret_code==kPopupReturnOK) {
        if (select_index == 1) {
            app_scr_set_infusion_duration_enter();
        }
        else {
            app_scr_set_infusion_speed_enter();
        }
    }
}

static void scr_set_clean_tube_dose_on_press_ok_key(KeyID key, int repeat_count)
{
    UINumberEdit* p = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit);
    set_user_infusion_para(kCleanTubeDose,p->value());

    InfusionMode infusion_mode=(InfusionMode)data_read_int(kDataInfusionModeSelection);
    if (infusion_mode == kAskEverytime) {
        app_alternative_popup_enter(get_string(kStrModeSelection), get_string(kStrSetSpeedMode), get_string(kStrSetTimeMode),0, scr_set_clean_on_select_infusion_mode_select);
    }else if(infusion_mode == kSetSpeedMode){
        app_scr_set_infusion_speed_enter();
    }else if(infusion_mode == kSetTimeMode){
        app_scr_set_infusion_duration_enter();
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
static void scr_set_clean_tube_dose_on_press_back_key(KeyID key, int repeat_count)
{
    screen_go_back();
}

/***********************************************
* Description:
*   handle left key
* Argument:
*
* Return:
*
************************************************/
static void scr_set_clean_tube_dose_on_press_left_key(KeyID key, int repeat_count)
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
		q->init(0,g_max_clean_tube_dose,1,10);
	}else{
		q->init(0,g_max_clean_tube_dose,1,1);
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
static void scr_set_clean_tube_dose_on_press_right_key(KeyID key, int repeat_count)
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
    if(repeat_count >= 10){
        q->init(0,g_max_clean_tube_dose,1,10);

    }else{
        q->init(0,g_max_clean_tube_dose,1,1);
    }
	ui_view_send_msg(p, msg);
}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void scr_set_clean_tube_dose_on_scr_create(Screen* scr)
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
static void scr_set_clean_tube_dose_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    //冲管药量
    scr_layout_load_standard(scr, get_string(kStrSetCleanTubeDose), get_string(kStrReturn), get_string(kStrConfirm));

    UINumberEdit* p = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit);
    p->set_value(get_user_infusion_para(kCleanTubeDose));
    g_max_clean_tube_dose = data_read_int(kDataMaxCleanTubeDose);
	p->init(0,g_max_clean_tube_dose,1,1);
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_set_clean_tube_dose_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, scr_set_clean_tube_dose_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, scr_set_clean_tube_dose_on_press_back_key);
    key_set_handler_with_repeat(kKeyLeft, kKeyEventPressed, scr_set_clean_tube_dose_on_press_left_key);
    key_set_handler_with_repeat(kKeyRight, kKeyEventPressed, scr_set_clean_tube_dose_on_press_right_key);

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
static void scr_set_clean_tube_dose_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_set_clean_tube_dose_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_set_clean_tube_dose_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_set_clean_tube_dose_on_scr_activate(scr, scr->view);
            break;
		case kUIMsgNotifyParent:
			if (msg->int_param==kUINotifyValueChanged &&
				msg->src->id()==kUI_IDNumberEdit){
				UINumberEdit* p = (UINumberEdit*)scr->view->get_child(kUI_IDNumberEdit);
                set_user_infusion_para(kCleanTubeDose,p->value());
			}
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
void scr_set_clean_tube_dose_enter(void)
{   
    screen_enter(kScreenSetCleanTubeDose, scr_set_clean_tube_dose_callback,NULL,
        g_set_clean_tube_dose_widgets, ssz_array_size(g_set_clean_tube_dose_widgets));
}
