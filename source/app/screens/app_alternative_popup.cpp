/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-16 xqzhao
* Initial revision.
*
************************************************/
#include "app_alternative_popup.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "simple_alloc.h"
#include "ui_image.h"
/************************************************
* Declaration
************************************************/
#define ALTERNATIVE_POPUP_SELECT1_X 48
#define ALTERNATIVE_POPUP_SELECT1_Y 6
#define ALTERNATIVE_POPUP_SELECT2_X 128
#define ALTERNATIVE_POPUP_SELECT2_Y 6
#define ALTERNATIVE_POPUP_SELECT_XSIZE 80
#define ALTERNATIVE_POPUP_SELECT_YSIZE 24

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_alternative_popup_widgets[] = {
	kWidgetAttrBase,kUIImage, kUI_IDImage, ALTERNATIVE_POPUP_SELECT1_X,ALTERNATIVE_POPUP_SELECT1_Y,0,0,

	kWidgetAttrBase,kUILable, kUI_IDLable1, 0,32,64,-1,
		kWidgetAttrTextID,kStrReturn, kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,
	kWidgetAttrBase,kUILable, kUI_IDLable2, -64,32,64,-1,
		kWidgetAttrTextID,kStrConfirm, kWidgetAttrAlign, UI_ALIGN_RIGHT | UI_ALIGN_VCENTER
};

/************************************************
* Function 
************************************************/

static void app_alternative_popup_update(Screen* scr)
{
	UIImage* p = M_ui_image(scr->view->get_child(kUI_IDImage));
	AlternativePopupObj* obj = (AlternativePopupObj*)scr->user_data;
	if (obj->select_index == 0) {
		p->set_img(get_image(kImgSelect1));
	}
	else {
		p->set_img(get_image(kImgSelect2));
	}
}

/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void app_alternative_popup_on_press_ok_key(KeyID key, int repeat_count)
{
	Screen* p = screen_current();
	AlternativePopupObj* obj = (AlternativePopupObj*)p->user_data;
	if (obj->callback) {
		obj->callback(kPopupReturnOK, obj->select_index);
	}
	if (obj->src) {
		screen_send_msg_with_para(obj->src, kUIMsgOK, obj->select_index, p);
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
static void app_alternative_popup_on_press_back_key(KeyID key, int repeat_count)
{
	Screen* p = screen_current();
	AlternativePopupObj* obj = (AlternativePopupObj*)p->user_data;
	if (obj->callback) {
		obj->callback(kPopupReturnCancel, obj->select_index);
	}
	if (obj->src) {
		screen_send_msg_with_para(obj->src, kUIMsgCancel, 0, p);
	}
	screen_delete(p);
}

static void app_alternative_popup_on_press_left_key(KeyID key, int repeat_count)
{
	AlternativePopupObj* obj = (AlternativePopupObj*)screen_current()->user_data;
	if (obj->select_index != 0) {
		obj->select_index = 0;
		app_alternative_popup_update(screen_current());
	}
}
static void app_alternative_popup_on_press_right_key(KeyID key, int repeat_count)
{
	AlternativePopupObj* obj = (AlternativePopupObj*)screen_current()->user_data;
	if (obj->select_index == 0) {
		obj->select_index = 1;
		app_alternative_popup_update(screen_current());
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
static void app_alternative_popup_on_scr_create(Screen* scr)
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
static void app_alternative_popup_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	UIImage* p = M_ui_image(scr_view->get_child(kUI_IDImage));
	p->set_auto_size(true);
	app_alternative_popup_update(scr);
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_alternative_popup_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_alternative_popup_on_press_ok_key);
	key_set_handler(kKeyBack, kKeyEventPressed, app_alternative_popup_on_press_back_key);
	key_set_handler(kKeyLeft, kKeyEventPressed, app_alternative_popup_on_press_left_key);
	key_set_handler(kKeyRight, kKeyEventPressed, app_alternative_popup_on_press_right_key);

	//set title
	AlternativePopupObj* obj = (AlternativePopupObj*)scr->user_data;
	screen_set_title(scr, obj->title);
}

static void app_alternative_popup_on_scr_force_draw(Screen* scr, UIView* scr_view)
{

	AlternativePopupObj* obj = (AlternativePopupObj*)scr->user_data;
	uint32_t text_mode;
	Rect rect;
	ui_set_font(DEFAULT_FONT);
	ui_set_bk_color(UI_BLACK);
	ui_set_text_color(UI_WHITE);
	//draw select1 text
	text_mode = UI_TEXT_ALIGN_CENTER;
	if (obj->select_index==0) {
		text_mode |= UI_TEXT_REVERSE;
	}
	ui_rect_init_by_size(rect, ALTERNATIVE_POPUP_SELECT1_X, ALTERNATIVE_POPUP_SELECT1_Y,
		ALTERNATIVE_POPUP_SELECT_XSIZE, ALTERNATIVE_POPUP_SELECT_YSIZE);
	ui_disp_text_in_rect(&rect, obj->select1_text, text_mode);
	//draw select2 text
	text_mode = UI_TEXT_ALIGN_CENTER;
	if (obj->select_index != 0) {
		text_mode |= UI_TEXT_REVERSE;
	}
	ui_rect_init_by_size(rect, ALTERNATIVE_POPUP_SELECT2_X, ALTERNATIVE_POPUP_SELECT2_Y,
		ALTERNATIVE_POPUP_SELECT_XSIZE, ALTERNATIVE_POPUP_SELECT_YSIZE);
	ui_disp_text_in_rect(&rect, obj->select2_text, text_mode);
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
static void app_alternative_popup_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_alternative_popup_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_alternative_popup_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_alternative_popup_on_scr_activate(scr, scr->view);
            break;
		case kUIMsgForceDrawAfterAllDraw:
			app_alternative_popup_on_scr_force_draw(scr, scr->view);
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

//show a alternative popup screen, when press ok or back, 
// it will use the callback and msg(kUIMsgOK or kUIMsgCancel) to notify user, then delete self
//return the popup screen
Screen* app_alternative_popup_enter_ex(
	const char* title, const char* select1_text, const char* select2_text, int first_select_index,
	Screen* src, AlternativePopupCallback callback)
{   
	AlternativePopupObj* p = (AlternativePopupObj*)salloc_new(kSAllocAlternativePopupObj, NULL);
	if (!p) {
		ssz_assert_fail();
		return NULL;
	}
	ssz_mem_zero(p, sizeof(*p));
	p->src = src;
	p->callback = callback;
	p->select1_text = select1_text;
	p->select2_text = select2_text;
	if (first_select_index>=0 && first_select_index<=1) {
		p->select_index = first_select_index;
	}
	if (title) {
		snprintf(p->title, APP_ALTERNATIVE_POPUP_TITLE_MAX_SIZE, "%s", title);
	}
    return screen_enter_ex(kScreenAlternativePopup, app_alternative_popup_callback,p,NULL,
        g_alternative_popup_widgets, ssz_array_size(g_alternative_popup_widgets),0);
}

//show a alternative popup screen, when press ok or back, 
// it will use the msg(kUIMsgOK and kUIMsgCancel) to notify user, then delete self
//return the popup screen
//Screen* app_alternative_popup_enter(const char* title,
//	const char* select1_text, const char* select2_text, int first_select_index,
//	Screen* src) {
//	return app_alternative_popup_enter_ex(title, select1_text, select2_text, first_select_index, src, NULL);
//}
//show a alternative popup screen, when press ok or back, 
// it will use the callback to notify user, then delete self
//return the popup screen
Screen* app_alternative_popup_enter(const char* title,
	const char* select1_text, const char* select2_text, int first_select_index,
	AlternativePopupCallback callback) {
	return app_alternative_popup_enter_ex(title, select1_text, select2_text, first_select_index, NULL, callback);
}
