/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-17 jlli
* Initial revision.
*
************************************************/
#include "app_scr_set_infusion_duration.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "ui_number_edit.h"
#include "app_alternative_popup.h"
#include "app_scr_set_peek_mode_duration.h"
#include "app_scr_task_confirm.h"
#include "ui_lable.h"
#include "data.h"
#include "app_scr_task_confirm.h"

/************************************************
* Declaration
************************************************/
extern  bool g_is_enter_from_speed_mode;
static  int  g_min_duration;

/************************************************
* Variable 
************************************************/
int static g_infusion_duration=5;
//widgets at this screen
static const int16_t g_scr_set_infusion_duration_widgets[] = {
    kWidgetAttrTitleTextID,kStrSetInfusionDuration,
    kWidgetAttrBase,kUILable, kUI_IDLable, 0,34,30,16,
    kWidgetAttrTextID,kStrReturn,kWidgetAttrAlign,UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable, 226,34,30,16,
    kWidgetAttrTextID,kStrConfirm,kWidgetAttrAlign,UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUILable, kUI_IDLable1, 165,20,52,20,
    kWidgetAttrTextID,kStrInfusionSpeed,kWidgetAttrAlign,UI_ALIGN_VCENTER,
    //kWidgetAttrBase,kUILable, kUI_IDLable, 163,36,10,10,
    //kWidgetAttrTextID,kStrDynamic2,kWidgetAttrAlign,UI_ALIGN_VCENTER|UI_ALIGN_RIGHT,
    kWidgetAttrBase,kUILable, kUI_IDLable2, 163,36,54,10,
    kWidgetAttrTextID,kStrDynamic1,kWidgetAttrAlign,UI_ALIGN_VCENTER|UI_ALIGN_RIGHT,
    kWidgetAttrBase,kUILable, kUI_IDLable3, 132,36,28,10,
    kWidgetAttrTextID,kStrHour,

    kWidgetAttrBase,kUILable, kUI_IDLable4, 27,4,106,42,kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,
    kWidgetAttrTextID,kStrDynamic3,kWidgetAttrAlign,UI_ALIGN_VCENTER|UI_ALIGN_RIGHT,
};

/************************************************
* Function 
************************************************/

static void app_scr_set_infusion_duration_on_select_peek_mode(PopupReturnCode ret_code, int select_index) {
	if (ret_code == kPopupReturnOK) {
		if (select_index == 1) {
			set_user_infusion_para(kIsPeekInfusionMode, true);
            g_is_enter_from_speed_mode=false;
			app_scr_set_peek_mode_duration_enter();
		}
		else {
			set_user_infusion_para(kIsPeekInfusionMode, false);
			app_scr_task_confirm_enter();
		}
	}
}

static void app_scr_set_infusion_duration_save_para()
{
    int infusion_speed;
    if(g_infusion_duration >= 1000) g_infusion_duration = g_infusion_duration / 10 * 10;
    infusion_speed = 10 * get_user_infusion_para(kTotalDose) * 10.0 / g_infusion_duration;
    set_user_infusion_para(kInfusionSpeed, infusion_speed);
    set_user_infusion_para(kInfusionDuration, g_infusion_duration * 6); //save duration time in minute
}

static void app_scr_set_infusion_duration_update_lable()
{
    UILable* lable=(UILable*)screen_current()->view->get_child(kUI_IDLable4);

    char *text = get_dynamic_string(kStrDynamic3);
    if(g_infusion_duration >= 1000)
        sprintf(text, "%d",g_infusion_duration / 10);
    else
        sprintf(text, "%d.%d",g_infusion_duration / 10, g_infusion_duration % 10);

    lable->set_text(text);
    lable->invalidate();

    char *string = get_dynamic_string(kStrDynamic1);
    float expect_speed = get_user_infusion_para(kTotalDose) * 10.0 / g_infusion_duration;

    if(g_infusion_duration > 0){
        if(expect_speed < 100)
            sprintf(string, "%d.%dml/h",(int)(expect_speed *10 / 10),((int)(expect_speed * 10)) % 10);
        else
            sprintf(string, "%dml/h",(int)(expect_speed * 10 / 10));
    } else {
        sprintf(string, "-mL/h");
    }

    lable= (UILable*)screen_current()->view->get_child(kUI_IDLable2);
    lable->invalidate();

}

/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void app_scr_set_infusion_duration_on_press_ok_key(KeyID key, int repeat_count)
{
    const char* text = get_string(kStrWhetherToAddPeekMode);
    app_scr_set_infusion_duration_save_para();

    bool is_enable_peek_mode=data_read_int(kDataPeekModeSwitch);
    if(is_enable_peek_mode == true){      
        app_alternative_popup_enter(text, get_string(kStrNo), get_string(kStrYes), 0, app_scr_set_infusion_duration_on_select_peek_mode);
    }else{
        app_scr_task_confirm_enter();
        set_user_infusion_para(kIsPeekInfusionMode,false);
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
static void app_scr_set_infusion_duration_on_press_back_key(KeyID key, int repeat_count)
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
static void app_scr_set_infusion_duration_on_press_left_key(KeyID key, int repeat_count)
{
	if (repeat_count >= 10) {
        if(g_infusion_duration >= 1100){
            g_infusion_duration -= 100;
        } else {
            if (g_infusion_duration >= 15) {
                if(g_infusion_duration > g_min_duration + 5)
                    g_infusion_duration -= 10;
            }
        }
	} else {
	    if (g_infusion_duration >= 1010) {
            g_infusion_duration -= 10;
        } else {
            if (g_infusion_duration >= g_min_duration + 5)
                g_infusion_duration -= 5;
        }
	}

    app_scr_set_infusion_duration_update_lable();
    app_scr_set_infusion_duration_save_para();

}

/***********************************************
* Description:
*   handle ok key
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_infusion_duration_on_press_right_key(KeyID key, int repeat_count)
{
    if (repeat_count >= 10) {
        if (g_infusion_duration >= 1000) {
            g_infusion_duration = g_infusion_duration / 10 * 10;
            if (g_infusion_duration <= get_user_infusion_para(kLimitInfusionDuration) - 100)
                if (10 * get_user_infusion_para(kTotalDose) * 10.0 / (g_infusion_duration + 100) >= 10)
                    g_infusion_duration += 100;
        } else {
            if (g_infusion_duration <= get_user_infusion_para(kLimitInfusionDuration) - 10)
                if (10 * get_user_infusion_para(kTotalDose) * 10.0 / (g_infusion_duration + 10) >= 10)
                    g_infusion_duration += 10;
        }
    } else {
        if (g_infusion_duration >= 1000) {
            g_infusion_duration=g_infusion_duration / 10 * 10;
            if (g_infusion_duration <= get_user_infusion_para(kLimitInfusionDuration) - 10)
                if (10 * get_user_infusion_para(kTotalDose) * 10.0 / (g_infusion_duration + 10) >= 10)
                    g_infusion_duration += 10;
        } else {
            if (g_infusion_duration <= get_user_infusion_para(kLimitInfusionDuration) - 5)
                if (10 * get_user_infusion_para(kTotalDose) * 10.0 / (g_infusion_duration + 5) >= 10)
                    g_infusion_duration += 5;
        }
    }

    app_scr_set_infusion_duration_update_lable();
    app_scr_set_infusion_duration_save_para();
}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_infusion_duration_on_scr_create(Screen* scr)
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
static void app_scr_set_infusion_duration_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    g_infusion_duration = get_user_infusion_para(kInfusionDuration) / 6;

    if (g_infusion_duration % 10 <= 4)    
        g_infusion_duration -= g_infusion_duration % 5;
    else
        g_infusion_duration += 5 - g_infusion_duration % 5;
    

    if(get_user_infusion_para(kTotalDose) * 1000 / 200 % 500 == 0)
        g_min_duration = get_user_infusion_para(kTotalDose) * 10 / 200;
    else   // step is 0.5h
        g_min_duration = get_user_infusion_para(kTotalDose) * 10 / 200 + (5 - get_user_infusion_para(kTotalDose) * 10 / 200 % 5);

    if(g_infusion_duration < g_min_duration)g_infusion_duration = g_min_duration;
    if(g_infusion_duration <= 0) g_infusion_duration = 10;
    app_scr_set_infusion_duration_update_lable(); 
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument: 
*
* Return:
*
************************************************/
static void app_scr_set_infusion_duration_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_set_infusion_duration_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_set_infusion_duration_on_press_back_key);
    key_set_handler_with_repeat(kKeyLeft, kKeyEventPressed, app_scr_set_infusion_duration_on_press_left_key);
    key_set_handler_with_repeat(kKeyRight, kKeyEventPressed, app_scr_set_infusion_duration_on_press_right_key);
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
static void app_scr_set_infusion_duration_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_set_infusion_duration_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_set_infusion_duration_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_set_infusion_duration_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
        case kUIMsgNotifyParent:
            if (msg->int_param==kUINotifyValueChanged
                && msg->src->id()==kUI_IDNumberEdit){
                UINumberEdit* p = (UINumberEdit*)scr->view->get_child(kUI_IDNumberEdit);
                set_user_infusion_para(kInfusionSpeed,p->value());

                char *time = get_dynamic_string(kStrDynamic1);
                float expect_time=(get_user_infusion_para(kTotalDose))*10.0/p->value();
                if(p->value()>0)
                    sprintf(time, "%d mL/h",(int)(expect_time));
                else
                    sprintf(time, "-mL/h");

                UILable* lable= (UILable*)scr->view->get_child(kUI_IDLable2);
                lable->invalidate();
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
void app_scr_set_infusion_duration_enter(void)
{   
    screen_enter(kScreenScrSetInfusionDuration, app_scr_set_infusion_duration_callback,NULL,
        g_scr_set_infusion_duration_widgets, ssz_array_size(g_scr_set_infusion_duration_widgets));
}
