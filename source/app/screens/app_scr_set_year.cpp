/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-10-18 jcwu
* Initial revision.
*
************************************************/
#include "app_scr_set_year.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_number_edit.h"
#include "ui_lable.h"
#include "mid_rtc.h"
#include "ssz_time_utility.h"
#include "record_log.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_scr_set_year_widgets[] = {
	kWidgetAttrBase,kUINumberEdit, kUI_IDNumberEdit, 60,4,140,42,
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
static void app_scr_set_year_on_press_ok_key(KeyID key, int repeat_count)
{
	UINumberEdit* ptr;
	SszDateTime curr_time;
	curr_time = *ssz_time_now();
    LogVar log_var;

	ptr = (UINumberEdit*)screen_current()->view->get_child(kUI_IDNumberEdit);
	curr_time.year = ptr->value();

	if (ssz_max_days_in_month(curr_time.year, curr_time.month) < curr_time.day) {
		curr_time.day = ssz_max_days_in_month(curr_time.year, curr_time.month);
	}
	rtc_set(&curr_time);
    log_var.type = kLogVarInteger;
    log_var.value = curr_time.year;
    record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
        kLogEventSetYear,&log_var, 1);
	screen_go_back_to_id(kScreenScrSetMenu);
}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void app_scr_set_year_on_press_back_key(KeyID key, int repeat_count)
{
	screen_go_back_to_id(kScreenScrSetDate);
}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void app_scr_set_year_on_scr_create(Screen* scr)
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
static void app_scr_set_year_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	SszDateTime curr_time;
	curr_time = *ssz_time_now();

	scr_layout_load_standard(scr, get_string(kStrSetYear), get_string(kStrReturn), get_string(kStrConfirm));

	UINumberEdit* q = (UINumberEdit*)scr_view->get_child(kUI_IDNumberEdit);
	q->set_value(curr_time.year);
	q->init(2017, MAX_YEAR, 1, 1);
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
static void app_scr_set_year_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_set_year_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_set_year_on_press_back_key);
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
static void app_scr_set_year_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_set_year_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_set_year_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_set_year_on_scr_activate(scr, scr->view);
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
void app_scr_set_year_enter(void)
{   
    screen_enter(kScreenScrSetYear, app_scr_set_year_callback,NULL,
        g_scr_set_year_widgets, ssz_array_size(g_scr_set_year_widgets));
}
