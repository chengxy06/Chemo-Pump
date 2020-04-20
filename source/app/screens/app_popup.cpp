/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-16 xqzhao
* Initial revision.
*
************************************************/
#include "app_popup.h"
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
static const int16_t g_popup_widgets[] = {
	kWidgetAttrBase,kUILable, kUI_IDLable, 0,0,-1,16,
		
	kWidgetAttrBase,kUILable, kUI_IDLable1, 0,16,-1,32,
	
	kWidgetAttrBase,kUILable, kUI_IDLable2, 0,48,64,-1,
	
//		kWidgetAttrTextID,kStrCancel, kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,
	kWidgetAttrBase,kUILable, kUI_IDLable3, -64,48,64,-1,
	
//		kWidgetAttrTextID,kStrConfirm, kWidgetAttrAlign, UI_ALIGN_RIGHT | UI_ALIGN_VCENTER
	kWidgetAttrBase,kUIImage, kUI_IDImage1, 8,16,56,32,

};

static const int16_t g_ask_popup_widgets[] = {
	kWidgetAttrBase,kUILable, kUI_IDLable1, 0,FIRST_LINE_Y,SCREEN_WINDOW_WIDTH,ONE_LINE_HEIGHT*2,
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
static void app_popup_on_press_ok_key(KeyID key, int repeat_count)
{
	Screen* p = screen_current();
	PopupObj* obj = (PopupObj*)p->user_data;
	if (obj->callback) {
		obj->callback(kPopupReturnOK);
	}
	if (obj->src) {
		screen_send_msg_no_para(obj->src, kUIMsgOK, p);
	}
	screen_delete(p);
}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void app_popup_on_press_back_key(KeyID key, int repeat_count)
{
	Screen* p = screen_current();
	PopupObj* obj = (PopupObj*)p->user_data;

	if (obj->popup_type == kPopupAsk) {
		if (obj->callback) {
			obj->callback(kPopupReturnCancel);
		}
		if (obj->src) {
			screen_send_msg_no_para(obj->src, kUIMsgCancel, p);
		}
		screen_delete(p);
	}
	else {
//		if (obj->callback) {
//			obj->callback(kPopupReturnOK);
//		}
//		if (obj->src) {
//			screen_send_msg_no_para(obj->src, kUIMsgOK, p);
//		}
//		screen_delete(p);
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
static void app_popup_on_scr_create(Screen* scr)
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
static void app_popup_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	PopupObj* obj = (PopupObj*)scr->user_data;
	UILable* title_lable = M_ui_lable(scr_view->get_child(kUI_IDLable));
	if (title_lable) {
		title_lable->set_bk_color(UI_WHITE);
		title_lable->set_text_color(UI_BLACK);
	}

	scr_layout_load_standard(scr, NULL, NULL, NULL);
	UILable* show_text_lable = M_ui_lable(scr_view->get_child(kUI_IDLable1));
	UILable* left_soft_key_lable = M_ui_lable(scr_view->get_child(kUI_IDLeftSoftKey));
	UILable* right_soft_key_lable = M_ui_lable(scr_view->get_child(kUI_IDRightSoftKey));
	UIImage* img = M_ui_image(scr_view->get_child(kUI_IDImage1));

	if (title_lable) {
		title_lable->set_text(obj->title);
	}
	show_text_lable->set_text(obj->show_text);

	switch(obj->popup_type){
		case kPopupWarning:
		{
			right_soft_key_lable->set_text(get_string(kStrConfirm));
			img->set_img(get_image(kImgWarning));
		}
			break;

		case kPopupError:
		{
			right_soft_key_lable->set_text(get_string(kStrConfirm));
			img->set_img(get_image(kImgError));
		}
			break;

		case kPopupInformation:
			{
			right_soft_key_lable->set_text(get_string(kStrOk));
			img->hide();
			}
				break;
		case kPopupRemind:
			{
			right_soft_key_lable->set_text(get_string(kStrConfirm));
			img->hide();
			}
			break;

		case kPopupAsk:
			left_soft_key_lable->set_text(get_string(kStrCancel));
			right_soft_key_lable->set_text(get_string(kStrConfirm));
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
static void app_popup_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_popup_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_popup_on_press_back_key);

	//set title
	PopupObj* obj = (PopupObj*)scr->user_data;
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
static void app_popup_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_popup_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_popup_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_popup_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
			salloc_delete(scr->user_data);
            break;
        default:
            break;
    }
    screen_on_msg_default(scr, msg);
}

//show a popup screen, when press ok or back, 
// it will use the callback or msg(kUIMsgOK or kUIMsgCancel) to notify user, then delete self
//return the popup screen
Screen * app_popup_enter_ex(PopupType popup_type, const char * title, const char * show_text, Screen* src, PopupCallback callback)
{
	PopupObj* p = (PopupObj*)salloc_new(kSAllocPopupObj, NULL);
	if (!p) {
		ssz_assert_fail();
		return NULL;
	}
	ssz_mem_zero(p, sizeof(*p));
	p->src = src;
	p->callback = callback;
	p->popup_type = popup_type;
	if (title) {
		snprintf(p->title, APP_POPUP_TITLE_MAX_SIZE, "%s", title);
	}else{
		switch(p->popup_type){
			case kPopupWarning:
				snprintf(p->title, APP_POPUP_TITLE_MAX_SIZE, "%s", "WARNING");
				break;

			case kPopupError:
				snprintf(p->title, APP_POPUP_TITLE_MAX_SIZE, "%s", "ERROR");
				break;

			case kPopupInformation:
				snprintf(p->title, APP_POPUP_TITLE_MAX_SIZE, "%s", "NOTICE");
				break;

			case kPopupRemind:
				snprintf(p->title, APP_POPUP_TITLE_MAX_SIZE, "%s", "CAUTION");
				break;

			case kPopupAsk:
				p->title[0] = 0;
				break;
		}
	}
	p->show_text = show_text;
	if (popup_type == kPopupAsk) {
		return screen_enter_ex(kScreenPopup, app_popup_callback, p, NULL,
			g_ask_popup_widgets, ssz_array_size(g_ask_popup_widgets), 0);
	}
	else {
		Rect screen_rect;
		ui_rect_init_by_size(screen_rect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		return screen_enter_ex(kScreenPopup, app_popup_callback, p, &screen_rect,
			g_popup_widgets, ssz_array_size(g_popup_widgets), 0);
	}
}

//show a popup screen, when press ok or back, 
// it will use the msg(kUIMsgOK and kUIMsgCancel) to notify user, then delete self
//return the popup screen
//Screen* app_popup_enter(PopupType popup_type, const char* title, const char* show_text, Screen* src) {
//	return app_popup_enter_ex(popup_type, title, show_text, src, NULL);
//}

//show a popup screen, when press ok or back, 
// it will use the callback to notify user, then delete self
//return the popup screen
Screen* app_popup_enter(PopupType popup_type, const char* title, const char* show_text, PopupCallback callback) {
	return app_popup_enter_ex(popup_type, title, show_text, NULL, callback);
}