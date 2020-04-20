/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-16 jlli
* Initial revision.
*
************************************************/
#include "app_scr_set_peek_mode_duration.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "ui_image.h"
#include "ui_number_edit.h"
#include "app_scr_set_peek_mode_speed.h"


/************************************************
* Declaration
************************************************/
extern  bool g_is_enter_from_speed_mode;

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_scr_set_peek_mode_duration_widgets[] = {
    kWidgetAttrTitleTextID,kStrSetPeekModeDuration,
    kWidgetAttrBase,kUILable, kUI_IDLable, 0,34,30,16,
    kWidgetAttrTextID,kStrReturn,kWidgetAttrAlign,UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable, 226,34,30,16,
    kWidgetAttrTextID,kStrConfirm,kWidgetAttrAlign,UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUIImage, kUI_IDImage, 23,15,10,18,
    kWidgetAttrImgID, kImgSetArrow,
    kWidgetAttrBase,kUILable, kUI_IDLable1, 105,20,20,10,
    kWidgetAttrTextID,kStrDynamic1,kWidgetAttrAlign,UI_ALIGN_VCENTER,kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,
    kWidgetAttrBase,kUILable, kUI_IDLable2, 90,36,20,12,kWidgetAttrFontHeight,SMALL_FONT_HEIGHT,
    kWidgetAttrTextID,kStrDynamic2,kWidgetAttrAlign,UI_ALIGN_VCENTER|UI_ALIGN_HCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable3, 200,36,20,12,kWidgetAttrFontHeight,SMALL_FONT_HEIGHT,
    kWidgetAttrTextID,kStrDynamic2,kWidgetAttrAlign,UI_ALIGN_VCENTER|UI_ALIGN_HCENTER,
    kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit2, 143,4,58,42,
    kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,
    kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit1, 33,4,58,42,
    kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,
};

static int g_cursor=1;

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
static void app_scr_set_peek_mode_duration_on_press_ok_key(KeyID key, int repeat_count)
{
    UIView* p;
    UINumberEdit* ptr;
    g_cursor ++;
    if(g_cursor == 2 ){
        p = screen_current_view()->get_child(kUI_IDNumberEdit2);
        p->set_focus();
        p = screen_current()->view->get_child(kUI_IDNumberEdit1);
        p->invalidate();
        p = screen_current()->view->get_child(kUI_IDLable1);
        p->invalidate();
        p = screen_current()->view->get_child(kUI_IDImage);
        p->set_pos(133,15);
    }else{
        ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
        set_user_infusion_para(kPeekInfusionStartTimePoint,ptr->value());
        ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
        set_user_infusion_para(kPeekInfusionEndTimePoint,ptr->value());

        app_scr_set_peek_mode_speed_enter();
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
static void app_scr_set_peek_mode_duration_on_press_back_key(KeyID key, int repeat_count)
{
    UIView* p;

    g_cursor --;
    if(g_cursor == 0 ){
        if(g_is_enter_from_speed_mode)
            screen_go_back_to_id(kScreenScrSetInfusionSpeed);
        else
            screen_go_back_to_id(kScreenScrSetInfusionDuration);
    }else{
        p = screen_current_view()->get_child(kUI_IDNumberEdit1);
        p->set_focus();
        p->invalidate();
        p = screen_current()->view->get_child(kUI_IDLable1);
        p->invalidate();
        p = screen_current()->view->get_child(kUI_IDImage);
        p->set_pos(23,15);
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
static void app_scr_set_peek_mode_duration_on_scr_create(Screen* scr)
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
static void app_scr_set_peek_mode_duration_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	UIImage* p = (UIImage*)scr_view->get_child(kUI_IDImage);
	p->enable_blink();

    UINumberEdit* q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit1);
    q->set_value(get_user_infusion_para(kPeekInfusionStartTimePoint));
    q->init(0,23,1,1);   
    q->set_show_all_digit_state(true);
    q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit2);
    q->set_value(get_user_infusion_para(kPeekInfusionEndTimePoint));
    q->init(0,23,1,1);
    q->set_show_all_digit_state(true);
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_peek_mode_duration_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_set_peek_mode_duration_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_set_peek_mode_duration_on_press_back_key);

    set_dynamic_string(kStrDynamic1,"-");
	set_dynamic_string(kStrDynamic2,":00");
    g_cursor=1;
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
static void app_scr_set_peek_mode_duration_callback(Screen* scr, ScreenMsg * msg)
{
    UINumberEdit* p;

    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_set_peek_mode_duration_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_set_peek_mode_duration_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_set_peek_mode_duration_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
        case kUIMsgNotifyParent:
            if (msg->int_param==kUINotifyValueChanged
                && msg->src->id()==kUI_IDNumberEdit1){
                p = (UINumberEdit*)scr->view->get_child(kUI_IDNumberEdit1);
                set_user_infusion_para(kPeekInfusionStartTimePoint,p->value());
            }
            if (msg->int_param==kUINotifyValueChanged
                && msg->src->id()==kUI_IDNumberEdit2){
                p = (UINumberEdit*)scr->view->get_child(kUI_IDNumberEdit2);
                set_user_infusion_para(kPeekInfusionEndTimePoint,p->value());
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
void app_scr_set_peek_mode_duration_enter(void)
{   
    screen_enter(kScreenScrSetPeekModeDuration, app_scr_set_peek_mode_duration_callback,NULL,
        g_scr_set_peek_mode_duration_widgets, ssz_array_size(g_scr_set_peek_mode_duration_widgets));
}
