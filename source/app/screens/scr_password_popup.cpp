/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-10-24 jlli
* Initial revision.
*
************************************************/
#include "scr_password_popup.h"
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
#include "simple_alloc.h"
#include "display.h"


#include "app_scr_set_infusion_speed.h"

/************************************************
* Declaration
************************************************/
int g_factory_password;
/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_password_popup_widgets[] = {
	kWidgetAttrBase,kUIImage, kUI_IDImage, 50,15,10,18, //75, 120
	kWidgetAttrImgID, kImgSetArrow,

	kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit2, 110,4,30,42,
	kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,

	kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit3, 155,4,30,42,
	kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,

    kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit1, 65,4,30,42,
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
static void scr_password_popup_on_press_ok_key(KeyID key, int repeat_count)
{
	Screen* p = screen_current();
    UIView* view;
    int password=0;

    PasswordPopupObj* obj = (PasswordPopupObj*)p->user_data;
	view = ui_view_focused();

    if (view->id() == kUI_IDNumberEdit1){
		view= screen_current_view()->get_child(kUI_IDNumberEdit2);
		view->set_focus();
		view = screen_current()->view->get_child(kUI_IDNumberEdit1);
		view->invalidate();
        view = screen_current()->view->get_child(kUI_IDNumberEdit3);
		view->invalidate();
		view = screen_current()->view->get_child(kUI_IDImage);
		view->set_pos(95, 15);
    }else if(view->id() == kUI_IDNumberEdit2){
		view= screen_current_view()->get_child(kUI_IDNumberEdit3);
		view->set_focus();
		view = screen_current()->view->get_child(kUI_IDNumberEdit1);
		view->invalidate();
        view = screen_current()->view->get_child(kUI_IDNumberEdit2);
		view->invalidate();
		view = screen_current()->view->get_child(kUI_IDImage);
		view->set_pos(140, 15);

    }else if(view->id() == kUI_IDNumberEdit3){

        UINumberEdit* ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit3);

        password = ptr->value();
        ptr = (UINumberEdit*)screen_current_view()->get_child(kUI_IDNumberEdit2);
        password += ptr->value()*10;
        ptr = (UINumberEdit*)screen_current_view()->get_child(kUI_IDNumberEdit1);
        password += ptr->value()*100;

        if(*obj->password != password){
            if (obj->callback) {
                obj->callback(kPopupReturnWrongPassword);
            }
            if (obj->src) {
                screen_send_msg_no_para(obj->src,kUIMsgWrongPassword, p);
            }
            app_popup_enter(kPopupRemind, NULL, get_string(kStrWrongPassword), NULL);//密码错误

        }else{
            if (obj->callback) {
                obj->callback(kPopupReturnOK);
            }
            if (obj->src) {
                screen_send_msg_no_para(obj->src, kUIMsgOK, p);
            }
            screen_delete(p);
        }
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
static void scr_password_popup_on_press_back_key(KeyID key, int repeat_count)
{
	Screen* p = screen_current();
    UIView* view;

    PasswordPopupObj* obj = (PasswordPopupObj*)p->user_data;
	view = ui_view_focused();

    if (view->id() == kUI_IDNumberEdit1){
        if (obj->callback) {
            obj->callback(kPopupReturnCancel);
        }
        if (obj->src) {
            screen_send_msg_no_para(obj->src, kUIMsgCancel, p);
        }
        //screen_delete(p);
        screen_go_back();
    }else if(view->id() == kUI_IDNumberEdit2){
		view= screen_current_view()->get_child(kUI_IDNumberEdit1);
		view->set_focus();
		view = screen_current()->view->get_child(kUI_IDNumberEdit3);
		view->invalidate();
        view = screen_current()->view->get_child(kUI_IDNumberEdit2);
		view->invalidate();
		view = screen_current()->view->get_child(kUI_IDImage);
		view->set_pos(50, 15);

    }else if(view->id() == kUI_IDNumberEdit3){
		view= screen_current_view()->get_child(kUI_IDNumberEdit2);
		view->set_focus();
		view = screen_current()->view->get_child(kUI_IDNumberEdit1);
		view->invalidate();
        view = screen_current()->view->get_child(kUI_IDNumberEdit3);
		view->invalidate();
		view = screen_current()->view->get_child(kUI_IDImage);
		view->set_pos(95, 15);
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
static void scr_password_popup_on_scr_create(Screen* scr)
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
static void scr_password_popup_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    scr_layout_load_standard(scr, get_string(kStrInputPasswordToUnlock), get_string(kStrReturn), get_string(kStrConfirm));

    UIImage* p = (UIImage*)scr_view->get_child(kUI_IDImage);
	p->enable_blink();

	UINumberEdit* q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit1);
	q->set_value(0);
	q->init(0, 9, 1, 1);
	q->set_show_all_digit_state(false);

	q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit2);
	q->set_value(0);
	q->init(0, 9, 1, 1);
	q->set_show_all_digit_state(false);

    q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit3);
	q->set_value(0);
	q->init(0, 9, 1, 1);
	q->set_show_all_digit_state(false);

}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_password_popup_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, scr_password_popup_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, scr_password_popup_on_press_back_key);
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
static void scr_password_popup_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_password_popup_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_password_popup_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_password_popup_on_scr_activate(scr, scr->view);
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

/***********************************************
* Description:
*   enter the screen
* Argument:
*
* Return:
*
************************************************/
Screen * scr_password_popup_enter_ex(int* password, Screen* src, PopupCallback callback)
{
    PasswordPopupObj* p = (PasswordPopupObj*)salloc_new(kSAllocPopupObj, NULL);
    //Rect screen_rect;
    if (!p) {
        ssz_assert_fail();
        return NULL;
    }
    ssz_mem_zero(p, sizeof(*p));
    p->src = src;
    p->callback = callback;
    p->password = password;

    return screen_enter_ex(kScreenPasswordPopup, scr_password_popup_callback,p,NULL,
        g_password_popup_widgets, ssz_array_size(g_password_popup_widgets), 0);
}

//Screen* scr_password_popup_enter(int* password, Screen* src)
//{
//	return scr_password_popup_enter_ex(password, src, NULL);
//}

//show a popup screen, when press ok or back, 
// it will use the callback to notify user, then delete self
//return the popup screen
Screen* scr_password_popup_enter(int* password, Screen* src,PopupCallback callback)
{
	return scr_password_popup_enter_ex(password, src, callback);
}

