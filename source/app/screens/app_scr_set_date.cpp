/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-10-18 jcwu
* Initial revision.
*
************************************************/
#include "app_scr_set_date.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_image.h"
#include "ui_number_edit.h"
#include "ui_lable.h"
#include "app_scr_set_year.h"
#include "mid_rtc.h"
#include "ssz_time_utility.h"
#include "record_log.h"

/************************************************
* Declaration
************************************************/
static SszDateTime g_set_data_time = {2017,10,24,11,11,11,2};
/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_scr_set_date_widgets[] = {
	kWidgetAttrBase,kUIImage, kUI_IDImage, 30,15,10,18,
	kWidgetAttrImgID, kImgSetArrow,

	kWidgetAttrBase,kUILable, kUI_IDLable1, 115,1,15,42, // 105,1,15,42,
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
static void app_scr_set_date_on_press_ok_key(KeyID key, int repeat_count)
{
	UIView* p;
	UINumberEdit* ptr;
	SszDate time_temp;
	p = ui_view_focused();
    LogVar log_var[2];
	if (p->id() == kUI_IDNumberEdit1)
	{
		p = screen_current_view()->get_child(kUI_IDNumberEdit2);
		p->set_focus();
		p = screen_current()->view->get_child(kUI_IDNumberEdit1);
		p->invalidate();
		p = screen_current()->view->get_child(kUI_IDLable1);
		p->invalidate();
		p = screen_current()->view->get_child(kUI_IDImage);
		p->set_pos(133, 15);		

		UILable*  t =(UILable*) screen_current_view()->get_child(kUI_IDLeftSoftKey); 
		t->set_text(get_string(kStrPrevious));
		t = (UILable*)screen_current_view()->get_child(kUI_IDRightSoftKey);
		t->set_text(get_string(kStrConfirm));

		ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_data_time.month = ptr->value();
		ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_data_time.day = ptr->value();
	}
	else if (p->id() == kUI_IDNumberEdit2)
	{
		ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_data_time.month = ptr->value();
		ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_data_time.day = ptr->value();
		time_temp.day = g_set_data_time.day;
		time_temp.month = g_set_data_time.month;
		time_temp.year = g_set_data_time.year;
		time_temp.weekday = g_set_data_time.weekday;
		rtc_set_date(&time_temp);

        log_var[0].type = kLogVarInteger;
        log_var[0].value = g_set_data_time.month;
        log_var[1].type = kLogVarInteger;
        log_var[1].value = g_set_data_time.day;
        record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
            kLogEventSetDate,log_var, ssz_array_size(log_var));

		app_scr_set_year_enter();
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
static void app_scr_set_date_on_press_back_key(KeyID key, int repeat_count)
{
	UIView* p;
	UINumberEdit* ptr;
	p = ui_view_focused();
	if (p->id() == kUI_IDNumberEdit1)
	{
		ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_data_time.month = ptr->value();
		ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_data_time.day = ptr->value();
		screen_go_back();
	}
	else if (p->id() == kUI_IDNumberEdit2)
	{
		p = screen_current_view()->get_child(kUI_IDNumberEdit1);
		p->set_focus();
		p->invalidate();
		p = screen_current()->view->get_child(kUI_IDLable1);
		p->invalidate();
		p = screen_current()->view->get_child(kUI_IDImage);
		p->set_pos(30, 15);
		ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_data_time.month = ptr->value();
		ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_data_time.day = ptr->value();

		UILable*  t = (UILable*)screen_current_view()->get_child(kUI_IDLeftSoftKey);
		t->set_text(get_string(kStrReturn));
		t = (UILable*)screen_current_view()->get_child(kUI_IDRightSoftKey);
		t->set_text(get_string(kStrNext));
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
static void app_scr_set_date_on_press_right_key(KeyID key, int repeat_count)
{
	UIView* p;
	UINumberEdit* q;
	UINumberEdit* d;

	p = ui_view_focused();
	if (p->id() == kUI_IDNumberEdit1)
	{
		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_data_time.month = q->value();
		g_set_data_time.month++;
		if (g_set_data_time.month > 12) {
			g_set_data_time.month = 1;
		}
		q->set_value(g_set_data_time.month);


		d = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_data_time.day = d->value();
		if(ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month) < g_set_data_time.day){
			g_set_data_time.day = ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month);
			d->set_value(g_set_data_time.day);
		}
	}
	else if (p->id() == kUI_IDNumberEdit2)
	{
		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_data_time.month = q->value();
		
		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_data_time.day = q->value();
		if (ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month) < g_set_data_time.day) {
			g_set_data_time.day = ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month);			
		}
		else if (ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month)== g_set_data_time.day){
			g_set_data_time.day = 1;
		}
		else if (ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month) > g_set_data_time.day){
			g_set_data_time.day++;
		}
		q->set_value(g_set_data_time.day);
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
static void app_scr_set_date_on_press_left_key(KeyID key, int repeat_count)
{
	UIView* p;
	UINumberEdit* q;
	UINumberEdit* d;

	p = ui_view_focused();
	if (p->id() == kUI_IDNumberEdit1)
	{
		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_data_time.month = q->value();
		if (g_set_data_time.month > 1) {
			g_set_data_time.month--;
		} else if (1 == g_set_data_time.month) {
			g_set_data_time.month = 12;
		}
		q->set_value(g_set_data_time.month);


		d = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_data_time.day = d->value();
		if (ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month) < g_set_data_time.day) {
			g_set_data_time.day = ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month);
			d->set_value(g_set_data_time.day);
		}
	}
	else if (p->id() == kUI_IDNumberEdit2)
	{
		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit1);
		g_set_data_time.month = q->value();

		q = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit2);
		g_set_data_time.day = q->value();
		if (ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month) < g_set_data_time.day) {
			g_set_data_time.day = ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month);
		}
		else if (ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month) >= g_set_data_time.day) {
			if (1 == g_set_data_time.day) {
				g_set_data_time.day = ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month);
			}
			else
			{
				g_set_data_time.day--;
			}
		}
		q->set_value(g_set_data_time.day);
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
static void app_scr_set_date_on_scr_create(Screen* scr)
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
static void app_scr_set_date_on_scr_widget_init(Screen* scr, UIView* scr_view)
{	
	rtc_scan();

	g_set_data_time = *ssz_time_now();
	scr_layout_load_standard(scr, get_string(kStrSetMonth), get_string(kStrReturn), get_string(kStrNext));

	UIImage* p = (UIImage*)scr_view->get_child(kUI_IDImage);
	p->enable_blink();

	UINumberEdit* q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit1);
	q->set_value(g_set_data_time.month);
	q->init(1, 12, 1, 1);
	q->set_show_all_digit_state(true);

	q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit2);
	q->set_value(g_set_data_time.day);
	q->init(1, ssz_max_days_in_month(g_set_data_time.year, g_set_data_time.month), 1, 1);
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
static void app_scr_set_date_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_set_date_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_set_date_on_press_back_key);
	key_set_handler(kKeyLeft, kKeyEventPressed, app_scr_set_date_on_press_left_key);
	key_set_handler(kKeyRight, kKeyEventPressed, app_scr_set_date_on_press_right_key);
	set_dynamic_string(kStrDynamic1, "-");
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
static void app_scr_set_date_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_set_date_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_set_date_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_set_date_on_scr_activate(scr, scr->view);
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
void app_scr_set_date_enter(void)
{   
    screen_enter(kScreenScrSetDate, app_scr_set_date_callback,NULL,
        g_scr_set_date_widgets, ssz_array_size(g_scr_set_date_widgets));
}
