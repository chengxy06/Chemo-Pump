/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-14 jlli
* Initial revision.
*
************************************************/
#include "app_scr_set_total_dose.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "ui_number_edit.h"
#include "app_scr_set_infusion_speed.h"
#include "scr_set_clean_tube_dose.h"
#include "param.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_scr_set_total_dose_widgets[] = {
    kWidgetAttrTitleTextID,kStrSetTotalDose,
    kWidgetAttrBase,kUILable, kUI_IDLable, 0,34,30,16,
    kWidgetAttrTextID,kStrReturn,kWidgetAttrAlign,UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable, 226,34,30,16,
    kWidgetAttrTextID,kStrConfirm,kWidgetAttrAlign,UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable, 175,36,14,10,
    kWidgetAttrTextID,kStrMililiter,
    kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit, 83,4,90,42,
    kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,
};

/************************************************
* Function 
************************************************/

/***********************************************
* Description:
*   handle ok key
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_total_dose_on_press_ok_key(KeyID key, int repeat_count)
{
    UINumberEdit* p = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit);
    set_user_infusion_para(kTotalDose,p->value());
    scr_set_clean_tube_dose_enter();
}
/***********************************************
* Description:
*   handle back key
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_total_dose_on_press_back_key(KeyID key, int repeat_count)
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
static void app_scr_set_total_dose_on_press_left_key(KeyID key, int repeat_count)
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
		q->init(MIN_TOTAL_DOSE_SET,MAX_TOTAL_DOSE_SET,1,10);
	}else{
		q->init(MIN_TOTAL_DOSE_SET,MAX_TOTAL_DOSE_SET,1,1);
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
static void app_scr_set_total_dose_on_press_right_key(KeyID key, int repeat_count)
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
        q->init(MIN_TOTAL_DOSE_SET,MAX_TOTAL_DOSE_SET,1,10);

    }else{
        q->init(MIN_TOTAL_DOSE_SET,MAX_TOTAL_DOSE_SET,1,1);
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
static void app_scr_set_total_dose_on_scr_create(Screen* scr)
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
static void app_scr_set_total_dose_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	UINumberEdit* p = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit);
    p->set_value(get_user_infusion_para(kTotalDose));
	p->init(MIN_TOTAL_DOSE_SET,MAX_TOTAL_DOSE_SET,1,1);
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_total_dose_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_set_total_dose_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_set_total_dose_on_press_back_key);
    key_set_handler_with_repeat(kKeyLeft, kKeyEventPressed, app_scr_set_total_dose_on_press_left_key);
    key_set_handler_with_repeat(kKeyRight, kKeyEventPressed, app_scr_set_total_dose_on_press_right_key);
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
static void app_scr_set_total_dose_callback(Screen* scr, ScreenMsg * msg)
{
	switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_set_total_dose_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_set_total_dose_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_set_total_dose_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
		case kUIMsgNotifyParent:
			if (msg->int_param==kUINotifyValueChanged &&
				msg->src->id()==kUI_IDNumberEdit){
				UINumberEdit* p = (UINumberEdit*)scr->view->get_child(kUI_IDNumberEdit);
                set_user_infusion_para(kTotalDose,p->value());
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
void app_scr_set_total_dose_enter(void)
{   
    screen_enter(kScreenScrSetTotalDose, app_scr_set_total_dose_callback,NULL,
        g_scr_set_total_dose_widgets, ssz_array_size(g_scr_set_total_dose_widgets));
}
