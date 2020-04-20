/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-16 xqzhao
* Initial revision.
*
************************************************/
#include "scr_unlock_tip.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "simple_alloc.h"
#include "ui_lable.h"
#include "display.h"
#include "ui_image.h"
#include "scr_layout.h"
#include "param.h"
#include "scr_tip_popup.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_unlock_tip_widgets[] = {
	kWidgetAttrBase,kUILable, kUI_IDLable, 0,0,-1,16,
		
	kWidgetAttrBase,kUILable, kUI_IDLable1, 0,16,-1,-1,
	
	//kWidgetAttrBase,kUILable, kUI_IDLable2, 0,48,64,-1,
	
//		kWidgetAttrTextID,kStrCancel, kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,
	//kWidgetAttrBase,kUILable, kUI_IDLable3, -64,48,64,-1,
	
//		kWidgetAttrTextID,kStrConfirm, kWidgetAttrAlign, UI_ALIGN_RIGHT | UI_ALIGN_VCENTER
	kWidgetAttrBase,kUIImage, kUI_IDImage1, 8,16,56,32,

};


/************************************************
* Function 
************************************************/

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...
* Return:
*
************************************************/
static void scr_unlock_tip_on_press_any_key(KeyID key, int repeat_count)
{
	if (key == kKeyRight) {
		unlock_keypad();
		screen_go_back();
	}
	else {
		scr_tip_popup_enter_ex(kScreenPressWrongToUnlockKeypad, kTipPopupInformation,
			NULL, get_string(kStrPressWrongKey), TIP_POPUP_SHOW_TIME_MS, NULL);
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
static void scr_unlock_tip_on_scr_create(Screen* scr)
{
	screen_set_timer_handler(kTimerAutoCloseUnlockTip, screen_go_back);
	timer_start_oneshot_after(kTimerAutoCloseUnlockTip, TIP_POPUP_SHOW_TIME_MS);
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
static void scr_unlock_tip_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	UILable* title_lable = M_ui_lable(scr_view->get_child(kUI_IDLable));
	title_lable->set_bk_color(UI_WHITE);
	title_lable->set_text_color(UI_BLACK);
	UILable* show_text_lable = M_ui_lable(scr_view->get_child(kUI_IDLable1));
	//UIImage* img = M_ui_image(scr_view->get_child(kUI_IDImage1));

	title_lable->set_text("NOTICE");
	show_text_lable->set_text(get_string(kStrPressKeyToUnlocked));

}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_unlock_tip_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
	for (int i = 0; i < kKeyIDMax; i++) {
		key_set_handler((KeyID)i, kKeyEventPressed, scr_unlock_tip_on_press_any_key);
	}
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
static void scr_unlock_tip_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_unlock_tip_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_unlock_tip_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_unlock_tip_on_scr_activate(scr, scr->view);
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

void scr_unlock_tip_enter()
{
	Rect screen_rect;
	ui_rect_init_by_size(screen_rect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	screen_enter_ex(kScreenKeypadUnlockTip, scr_unlock_tip_callback, NULL, &screen_rect,
		g_unlock_tip_widgets, ssz_array_size(g_unlock_tip_widgets), 
		SCREEN_MODE_AUTO_DELETE_WHEN_DEACTIVE);
}
