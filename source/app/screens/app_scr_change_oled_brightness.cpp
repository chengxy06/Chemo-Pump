/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2018-01-02 jcwu
* Initial revision.
*
************************************************/
#include "app_scr_change_oled_brightness.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_number_edit.h"
#include "ui_image.h"
#include "app_popup.h"
#include "data.h"
#include "drv_oled.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_scr_change_oled_brightness_widgets[] = {
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
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void app_scr_change_oled_brightness_on_press_ok_key(KeyID key, int repeat_count)
{
	//UIView* view;
	//view = ui_view_focused();
	
	int oled_brightness = 0;
    UINumberEdit* p = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit);
	oled_brightness = p->value();

	oled_brightness = data_write_int(kDataBrightness, oled_brightness);
	if (kSuccess == oled_brightness) {
		drv_oled_set_contrast_current((uint8_t)p->value()); // 
		app_popup_enter(kPopupRemind, NULL, "Change Brightness Success", NULL);
	}
	else if (kError == oled_brightness) {
		app_popup_enter(kPopupInformation, NULL, "Change Brightness Fail", NULL);
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
static void app_scr_change_oled_brightness_on_press_back_key(KeyID key, int repeat_count)
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
static void app_scr_change_oled_brightness_on_press_left_key(KeyID key, int repeat_count)
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
		q->init(0,255,1,10);
	}else{
		q->init(0,255,1,1);
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
static void app_scr_change_oled_brightness_on_press_right_key(KeyID key, int repeat_count)
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
        q->init(0,255,1,10);

    }else{
        q->init(0,255,1,1);
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
static void app_scr_change_oled_brightness_on_scr_create(Screen* scr)
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
static void app_scr_change_oled_brightness_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
  //  scr_layout_load_standard(scr, NULL, get_string(kStrReturn), NULL);
	scr_layout_load_standard(scr, get_string(kStrInputNewOledBrightness), get_string(kStrReturn), get_string(kStrConfirm));

	UINumberEdit* p = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit);
    p->set_value(data_read_int(kDataBrightness));
	p->init(0,255,1,1); 
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_scr_change_oled_brightness_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_change_oled_brightness_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_change_oled_brightness_on_press_back_key);
    key_set_handler_with_repeat(kKeyLeft, kKeyEventPressed, app_scr_change_oled_brightness_on_press_left_key);
    key_set_handler_with_repeat(kKeyRight, kKeyEventPressed, app_scr_change_oled_brightness_on_press_right_key);	
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
static void app_scr_change_oled_brightness_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_change_oled_brightness_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_change_oled_brightness_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_change_oled_brightness_on_scr_activate(scr, scr->view);
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
void app_scr_change_oled_brightness_enter(void)
{   
    screen_enter(kScreenScrChangeOledBrightness, app_scr_change_oled_brightness_callback,NULL,
        g_scr_change_oled_brightness_widgets, ssz_array_size(g_scr_change_oled_brightness_widgets));
}
