/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-16 xqzhao
* Initial revision.
*
************************************************/
#include "scr_tip_popup.h"
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

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_tip_popup_widgets[] = {
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
static void scr_tip_popup_on_press_any_key(KeyID key, int repeat_count)
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
static void scr_tip_popup_on_scr_create(Screen* scr)
{
	TipPopupObj* obj = (TipPopupObj*)scr->user_data;
	if (obj->auto_close_time_ms>0) {
		screen_set_timer_handler(kTimerAutoCloseTipPopup, screen_go_back);
		timer_start_oneshot_after(kTimerAutoCloseTipPopup, obj->auto_close_time_ms);
	}
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
static void scr_tip_popup_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	TipPopupObj* obj = (TipPopupObj*)scr->user_data;
	UILable* title_lable = M_ui_lable(scr_view->get_child(kUI_IDLable));
	title_lable->set_bk_color(UI_WHITE);
	title_lable->set_text_color(UI_BLACK);
	UILable* show_text_lable = M_ui_lable(scr_view->get_child(kUI_IDLable1));
	UILable* left_soft_key_lable = M_ui_lable(scr_view->get_child(kUI_IDLable2));
	UILable* right_soft_key_lable = M_ui_lable(scr_view->get_child(kUI_IDLable3));
	UIImage* img = M_ui_image(scr_view->get_child(kUI_IDImage1));

	title_lable->set_text(obj->title);
	show_text_lable->set_text(obj->show_text);

	switch(obj->popup_type){
		case kTipPopupWarning:
		{
			img->set_img(get_image(kImgWarning));
		}
			break;

		case kTipPopupError:
		{
			img->set_img(get_image(kImgError));
		}
			break;

		case kTipPopupInformation:
			{
			img->hide();
			}
				break;
		case kTipPopupRemind:
			{
			img->hide();
			}
			break;
	}
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_tip_popup_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
	for (int i = 0; i < kKeyIDMax; i++) {
		key_set_handler((KeyID)i, kKeyEventPressed, scr_tip_popup_on_press_any_key);
	}

	//set title
	TipPopupObj* obj = (TipPopupObj*)scr->user_data;
	screen_set_title(scr, obj->title);
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
static void scr_tip_popup_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_tip_popup_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_tip_popup_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_tip_popup_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
		{
			Screen* p = scr;
			TipPopupObj* obj = (TipPopupObj*)p->user_data;
			if (obj->callback) {
				obj->callback();
			}
			break;
		}
        case kUIMsgScrDestroy:
			salloc_delete(scr->user_data);
            break;
        default:
            break;
    }
    screen_on_msg_default(scr, msg);
}

//show a tip popup screen, it will close for timer out, press any key or new screen enter
// when close it will use the callback to notify user, then delete self
//return the popup screen
Screen* scr_tip_popup_enter(TipPopupType popup_type, const char* title, const char* show_text, TipPopupCallback callback) {
	return scr_tip_popup_enter_ex(kScreenTipPopup, popup_type, title, show_text, TIP_POPUP_SHOW_TIME_MS, callback);
}
Screen * scr_tip_popup_enter_ex(ScreenID screen_id, TipPopupType popup_type, const char * title, const char * show_text, int auto_close_time_ms, TipPopupCallback callback)
{
	TipPopupObj* p = (TipPopupObj*)salloc_new(kSAllocTipPopupObj, NULL);
	if (!p) {
		ssz_assert_fail();
		return NULL;
	}
	ssz_mem_zero(p, sizeof(*p));
	p->callback = callback;
	p->popup_type = popup_type;
	p->auto_close_time_ms = auto_close_time_ms;
	if (title) {
		snprintf(p->title, TIP_POPUP_TITLE_MAX_SIZE, "%s", title);
	}else{
		switch(p->popup_type){
			case kTipPopupWarning:
				snprintf(p->title, TIP_POPUP_TITLE_MAX_SIZE, "%s", "WARNING");
				break;

			case kTipPopupError:
				snprintf(p->title, TIP_POPUP_TITLE_MAX_SIZE, "%s", "ERROR");
				break;

			case kTipPopupInformation:
				snprintf(p->title, TIP_POPUP_TITLE_MAX_SIZE, "%s", "NOTICE");
				break;

			case kTipPopupRemind:
				snprintf(p->title, TIP_POPUP_TITLE_MAX_SIZE, "%s", "CAUTION");
				break;
		}
	}
	p->show_text = show_text;
	Rect screen_rect;
	ui_rect_init_by_size(screen_rect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	return screen_enter_ex(screen_id, scr_tip_popup_callback, p, &screen_rect,
		g_tip_popup_widgets, ssz_array_size(g_tip_popup_widgets), 
		SCREEN_MODE_AUTO_DELETE_WHEN_DEACTIVE);
}
