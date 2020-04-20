/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-10-12 jcwu
* Initial revision.
*
************************************************/
#include "app_scr_set_time.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "string_data.h"
#include "ui_number_edit.h"
#include "ui_image.h"
#include "ui_lable.h"
#include "mid_rtc.h"
#include "app_scr_set_date.h"
#include "ui_lable.h"
#include "record_log.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

static SszDateTime g_set_time;


//widgets at this screen
static const int16_t g_scr_set_time_widgets[] = {
//   kWidgetAttrNull
//	kWidgetAttrBase,kUIImageList, kUI_IDImageList,30,5,40,40,
    kWidgetAttrBase,kUIImage, kUI_IDImage, 30,15,10,18,
    kWidgetAttrImgID, kImgSetArrow,

    kWidgetAttrBase,kUILable, kUI_IDLable1, 110,1,15,42,
    kWidgetAttrTextID,kStrDynamic1,kWidgetAttrAlign,UI_ALIGN_VCENTER,kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,

    kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit2, 143,4,58,42,
    kWidgetAttrFontHeight,LARGE_FONT_HEIGHT,
    kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit1, 45,4,58,42,
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
static void app_scr_set_time_on_press_ok_key(KeyID key, int repeat_count)
{
    UIView* p;
    UINumberEdit* ptr;
	SszTime curr_time;

	p = ui_view_focused();
	if (p->id() == kUI_IDNumberEdit1)
	{
		p = screen_current_view()->get_child(kUI_IDNumberEdit2);
		p->set_focus();
		//p = screen_current()->view->get_child(kUI_IDNumberEdit1);
		//p->invalidate();
		//p = screen_current()->view->get_child(kUI_IDLable1);
		//p->invalidate();
		p = screen_current()->view->get_child(kUI_IDImage);
		p->set_pos(133, 15);

		UILable*  t = (UILable*)screen_current_view()->get_child(kUI_IDLeftSoftKey);
		t->set_text(get_string(kStrPrevious));
		t = (UILable*)screen_current_view()->get_child(kUI_IDRightSoftKey);
		t->set_text(get_string(kStrConfirm));
	}
	else if (p->id() == kUI_IDNumberEdit2)
	{
        LogVar log_var[2];

        ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		curr_time.hour = ptr->value();
		ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);		
		curr_time.minute = ptr->value();
		rtc_set_time(&curr_time);

        log_var[0].type = kLogVarInteger;
        log_var[0].value = curr_time.hour;
        log_var[1].type = kLogVarInteger;
        log_var[1].value = curr_time.minute;
        record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
            kLogEventSetTime,log_var, ssz_array_size(log_var));
        
		app_scr_set_date_enter();
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
static void app_scr_set_time_on_press_back_key(KeyID key, int repeat_count)
{
    UIView* p;
	p = ui_view_focused();
	if (p->id() == kUI_IDNumberEdit1)
	{
		screen_go_back(); //screen_go_back_to_id(kScreenScrSetMenu);
	}
	else if (p->id() == kUI_IDNumberEdit2)
	{
		p = screen_current_view()->get_child(kUI_IDNumberEdit1);
		p->set_focus();
		//p->invalidate();
		//p = screen_current()->view->get_child(kUI_IDLable1);
		//p->invalidate();
		p = screen_current()->view->get_child(kUI_IDImage);
		p->set_pos(30, 15); 

		UILable*  t = (UILable*)screen_current_view()->get_child(kUI_IDLeftSoftKey);
		t->set_text(get_string(kStrReturn));
		t = (UILable*)screen_current_view()->get_child(kUI_IDRightSoftKey);
		t->set_text(get_string(kStrNext));
	}
}
/***********************************************
* Description:
*   handle left key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...
* Return:
*
************************************************/
static void app_scr_set_time_on_press_left_key(KeyID key, int repeat_count)
{
	UIView* p;
	UINumberEdit* q;
	p = ui_view_focused();
	if (p->id() == kUI_IDNumberEdit1)
	{
		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_time.hour = q->value();
		if (g_set_time.hour > 0) {
			g_set_time.hour--;
		}
		else if (0 == g_set_time.hour) {
			g_set_time.hour = 23;
		}
		q->set_value(g_set_time.hour);
	}
	else if (p->id() == kUI_IDNumberEdit2)
	{
		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_time.minute = q->value();
		if (g_set_time.minute > 0) {
			g_set_time.minute--;
		}
		else if (0 == g_set_time.minute) {
			g_set_time.minute = 59;
		}
		q->set_value(g_set_time.minute);
	}
}

/***********************************************
* Description:
*   handle right key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...
* Return:
*
************************************************/
static void app_scr_set_time_on_press_right_key(KeyID key, int repeat_count)
{
	UIView* p;
	UINumberEdit* q;
	p = ui_view_focused();
	if (p->id() == kUI_IDNumberEdit1)
	{
		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_time.hour = q->value();
		if (g_set_time.hour < 23) {
			g_set_time.hour++;
		}
		else if (23 == g_set_time.hour) {
			g_set_time.hour = 0;
		}
		q->set_value(g_set_time.hour);
	}
	else if (p->id() == kUI_IDNumberEdit2)
	{
		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_time.minute = q->value();
		if (g_set_time.minute < 59) {
			g_set_time.minute++;
		}
		else if (59 == g_set_time.minute) {
			g_set_time.minute = 0;
		}
		q->set_value(g_set_time.minute);
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
static void app_scr_set_time_on_scr_create(Screen* scr)
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
static void app_scr_set_time_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	g_set_time = *ssz_time_now();
	scr_layout_load_standard(scr, get_string(kStrSetHour ), get_string(kStrReturn), get_string(kStrNext));

	UIImage* p = (UIImage*)scr_view->get_child(kUI_IDImage);
	p->enable_blink();
	 
	UINumberEdit* q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit1);
	q->set_value(g_set_time.hour);
	q->init(0,23,1,1);   
	q->set_show_all_digit_state(true);


	q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit2);
	q->set_value(g_set_time.minute);
	q->init(0,59,1,1);
	q->set_show_all_digit_state(true);

	UILable*  t = M_ui_lable(scr->view->get_child(kUI_IDLeftSoftKey));
	t->set_size(40, LEFT_SOFT_KEY_HEIGHT);
	UILable*  d = M_ui_lable(scr->view->get_child(kUI_IDRightSoftKey));
	d->set_size(45, RIGHT_SOFT_KEY_HEIGHT);
	d->set_pos(209, 34);
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_time_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_set_time_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_set_time_on_press_back_key);
	key_set_handler(kKeyLeft, kKeyEventPressed, app_scr_set_time_on_press_left_key);
	key_set_handler(kKeyRight, kKeyEventPressed, app_scr_set_time_on_press_right_key);
	set_dynamic_string(kStrDynamic1,":");
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
static void app_scr_set_time_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_set_time_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_set_time_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_set_time_on_scr_activate(scr, scr->view);
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
void app_scr_set_time_enter(void)
{   
    screen_enter(kScreenScrSetTime, app_scr_set_time_callback,NULL,
        g_scr_set_time_widgets, ssz_array_size(g_scr_set_time_widgets));
}
