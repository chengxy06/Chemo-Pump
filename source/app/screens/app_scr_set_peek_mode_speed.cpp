/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-17 jlli
* Initial revision.
*
************************************************/
#include "app_scr_set_peek_mode_speed.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "ui_number_edit.h"
#include "app_scr_task_confirm.h"
#include "ui_lable.h"


/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
int static g_peak_mode_infusion_speed;

//widgets at this screen
static const int16_t g_scr_set_peek_mode_speed_widgets[] = {
    kWidgetAttrTitleTextID,kStrPeekSpeed,
    kWidgetAttrBase,kUILable, kUI_IDLable, 0,34,30,16,
    kWidgetAttrTextID,kStrReturn,kWidgetAttrAlign,UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable, 226,34,30,16,
    kWidgetAttrTextID,kStrConfirm,kWidgetAttrAlign,UI_ALIGN_VCENTER,

    kWidgetAttrBase,kUILable, kUI_IDLable1, 165,19,52,16,
    kWidgetAttrTextID,kStrTimeDuration,kWidgetAttrAlign,UI_ALIGN_VCENTER|UI_ALIGN_LEFT,

    kWidgetAttrBase,kUILable, kUI_IDLable2, 165,33,52,16,
    kWidgetAttrTextID,kStrDynamic1,kWidgetAttrAlign,UI_ALIGN_VCENTER|UI_ALIGN_LEFT,

    kWidgetAttrBase,kUILable, kUI_IDLable3, 134,36,28,10,
    kWidgetAttrTextID,kStrMililiterPerHour1,

    kWidgetAttrBase,kUILable, kUI_IDLable4, 30,4,105,42,kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,
    kWidgetAttrTextID,kStrDynamic2,kWidgetAttrAlign,UI_ALIGN_VCENTER|UI_ALIGN_RIGHT,

};

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
static void app_scr_set_peek_mode_speed_on_press_ok_key(KeyID key, int repeat_count)
{
    if(g_peak_mode_infusion_speed >= 1000) g_peak_mode_infusion_speed=g_peak_mode_infusion_speed/10*10;
    set_user_infusion_para(kPeekInfusionSpeed,g_peak_mode_infusion_speed);

    app_scr_task_confirm_enter();
}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void app_scr_set_peek_mode_speed_on_press_back_key(KeyID key, int repeat_count)
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
static void app_scr_set_peek_mode_speed_on_press_left_key(KeyID key, int repeat_count)
{
    UILable* lable=(UILable*)screen_current()->view->get_child(kUI_IDLable4);

	if (repeat_count >= 10) {
        if (g_peak_mode_infusion_speed >= 1100) {
            g_peak_mode_infusion_speed -= 100;
        } else {
            if (g_peak_mode_infusion_speed >= 11)
                g_peak_mode_infusion_speed -= 10;
            if (g_peak_mode_infusion_speed < 10)
                g_peak_mode_infusion_speed = 10;

        }
	} else {
	    if (g_peak_mode_infusion_speed >= 1010) {
            g_peak_mode_infusion_speed -= 10;
        } else {
            if(g_peak_mode_infusion_speed >= 11)
                g_peak_mode_infusion_speed -= 1;
        }
	}

    char *text = get_dynamic_string(kStrDynamic2);
    if (g_peak_mode_infusion_speed >= 1000)
        sprintf(text, "%d",g_peak_mode_infusion_speed / 10);
    else
        sprintf(text, "%d.%d",g_peak_mode_infusion_speed / 10, g_peak_mode_infusion_speed % 10);

    lable->set_text(text);
    lable->invalidate();

}

/***********************************************
* Description:
*   handle ok key
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_peek_mode_speed_on_press_right_key(KeyID key, int repeat_count)
{
    UILable* lable=(UILable*)screen_current()->view->get_child(kUI_IDLable4);

    if (repeat_count >= 10){
        if (g_peak_mode_infusion_speed >= 1000) {
            g_peak_mode_infusion_speed=g_peak_mode_infusion_speed/10*10;
            if(g_peak_mode_infusion_speed <= get_user_infusion_para(kLimitInfusionSpeed) - 100)
                g_peak_mode_infusion_speed += 100;
        } else {
            if (g_peak_mode_infusion_speed <= get_user_infusion_para(kLimitInfusionSpeed) - 10)
                g_peak_mode_infusion_speed += 10;
        }
    } else {
        if (g_peak_mode_infusion_speed >= 1000) {
            g_peak_mode_infusion_speed = g_peak_mode_infusion_speed / 10 * 10;
            if (g_peak_mode_infusion_speed <= get_user_infusion_para(kLimitInfusionSpeed) - 10)
                g_peak_mode_infusion_speed += 10;
        } else {
            if (g_peak_mode_infusion_speed <= get_user_infusion_para(kLimitInfusionSpeed) - 1)
                g_peak_mode_infusion_speed += 1;
        }
    }

    char *text = get_dynamic_string(kStrDynamic2);
    if(g_peak_mode_infusion_speed >= 1000)
        sprintf(text, "%d",g_peak_mode_infusion_speed/10);
    else
        sprintf(text, "%d.%d",g_peak_mode_infusion_speed/10,g_peak_mode_infusion_speed%10);

    lable->set_text(text);
    lable->invalidate();
}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_peek_mode_speed_on_scr_create(Screen* scr)
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
static void app_scr_set_peek_mode_speed_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    UILable* lable=(UILable*)screen_current()->view->get_child(kUI_IDLable4);
    char *string = get_dynamic_string(kStrDynamic2);

    g_peak_mode_infusion_speed = get_user_infusion_para(kPeekInfusionSpeed);
    if(g_peak_mode_infusion_speed < 10) g_peak_mode_infusion_speed = 10;

    if(g_peak_mode_infusion_speed >= 1000)
        sprintf(string, "%d",g_peak_mode_infusion_speed / 10);
    else
        sprintf(string, "%d.%d",g_peak_mode_infusion_speed / 10,g_peak_mode_infusion_speed % 10);

    lable->set_text(string);
    lable->invalidate();
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_peek_mode_speed_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_set_peek_mode_speed_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_set_peek_mode_speed_on_press_back_key);
    key_set_handler_with_repeat(kKeyLeft, kKeyEventPressed, app_scr_set_peek_mode_speed_on_press_left_key);
    key_set_handler_with_repeat(kKeyRight, kKeyEventPressed, app_scr_set_peek_mode_speed_on_press_right_key);

    char *p = get_dynamic_string(kStrDynamic1);
    sprintf(p, "%02d-%02d\xe6\x97\xb6",get_user_infusion_para(kPeekInfusionStartTimePoint),
            get_user_infusion_para(kPeekInfusionEndTimePoint));
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
static void app_scr_set_peek_mode_speed_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_set_peek_mode_speed_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_set_peek_mode_speed_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_set_peek_mode_speed_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
        case kUIMsgNotifyParent:
            if (msg->int_param==kUINotifyValueChanged
                && msg->src->id()==kUI_IDNumberEdit){
                UINumberEdit* p = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit);
                set_user_infusion_para(kPeekInfusionSpeed, p->value());
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
void app_scr_set_peek_mode_speed_enter(void)
{   
    screen_enter(kScreenScrSetPeekModeSpeed, app_scr_set_peek_mode_speed_callback,NULL,
        g_scr_set_peek_mode_speed_widgets, ssz_array_size(g_scr_set_peek_mode_speed_widgets));
}
