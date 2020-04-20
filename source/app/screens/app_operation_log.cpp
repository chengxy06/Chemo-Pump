/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2018-02-27 jlli
* Initial revision.
*
************************************************/
#include "app_operation_log.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_list_pos_indicator.h"
#include "ui_lable.h"
#include "record_log.h"
#include "record_log_text.h"
#include "param.h"
#include "ssz_time_utility.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_operation_log_widgets[] = {
	kWidgetAttrBase,kUILable, kUI_IDLable, 0,FIRST_LINE_Y,SCREEN_WIDTH,ONE_LINE_HEIGHT*2,
		kWidgetAttrAlign, UI_TEXT_ALIGN_LEFT| UI_TEXT_ALIGN_TOP,
	kWidgetAttrBase,kUIListPosIndicator, kUI_IDListPosIndicator, 85,LEFT_SOFT_KEY_Y,85,LEFT_SOFT_KEY_HEIGHT,
};
static int g_operation_log_view_index;
static char g_operation_log_show_text[LOG_TEXT_MAX_SIZE];

/************************************************
* Function 
************************************************/
static void app_operation_log_view_log(Screen* scr, int log_index)
{
	UIListPosIndicator* p = M_ui_list_pos_indicator(scr->view->get_child(kUI_IDListPosIndicator));
	UILable* lable = M_ui_lable(scr->view->get_child(kUI_IDLable));
	LogOneRecord one_rec;
	char title[30];
	char show_text[LOG_TEXT_MAX_SIZE];
	SszDateTime tim;

	result_t ret = record_log_get(kOperationLog, log_index, &one_rec);
	if (ret == kSuccess) {
		ssz_seconds_to_time(one_rec.occur_time, &tim);
		snprintf(title, sizeof(title), "%s %d-%d-%d", log_event_type_to_const_str(one_rec.event_type),
			tim.year, tim.month, tim.day);
		record_log_to_str(&one_rec, show_text, sizeof(show_text));
		snprintf(g_operation_log_show_text, sizeof(g_operation_log_show_text),
			"%02d:%02d %s", tim.hour,tim.minute, show_text);
	}
	else if (ret == kChecksumWrong) {
		snprintf(title, sizeof(title), "%s", log_event_type_to_const_str(LOG_EVENT_TYPE_INFO));
		snprintf(g_operation_log_show_text, sizeof(g_operation_log_show_text),
			"%s", log_event_id_to_const_str(kLogEventLost));
	}
	else {
		return;
	}

	p->set_value(log_index+1);
	screen_set_title(scr, title);
	lable->set_text_and_force_invalid(g_operation_log_show_text);

}

static void app_operation_log_on_press_left_key(KeyID key, int repeat_count)
{
	if (g_operation_log_view_index>0) {
		g_operation_log_view_index--;
		app_operation_log_view_log(screen_current(), g_operation_log_view_index);
	}
}

static void app_operation_log_on_press_right_key(KeyID key, int repeat_count)
{
	if (g_operation_log_view_index+1 < record_log_size(kOperationLog)) {
		g_operation_log_view_index++;
		app_operation_log_view_log(screen_current(), g_operation_log_view_index);
	}
}
/***********************************************
* Description:
*   handle ok key
* Argument:
*
* Return:
*
************************************************/
//static void app_history_log_on_press_ok_key(KeyID key, int repeat_count)
//{
//
//}

/***********************************************
* Description:
*   handle back key
* Argument:
*
* Return:
*
************************************************/
static void app_operation_log_on_press_back_key(KeyID key, int repeat_count)
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
static void app_operation_log_on_scr_create(Screen* scr)
{
	g_operation_log_view_index = record_log_size(kOperationLog)-1;
	if (g_operation_log_view_index < 0) {
		g_operation_log_view_index = 0;
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
static void app_operation_log_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	scr_layout_load_standard(scr, NULL, get_string(kStrReturn), NULL);
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_operation_log_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    //key_set_handler(kKeyOK, kKeyEventPressed, app_history_log_on_press_ok_key);
	key_set_handler(kKeyBack, kKeyEventPressed, app_operation_log_on_press_back_key);
	key_set_handler(kKeyLeft, kKeyEventPressed, app_operation_log_on_press_left_key);
	key_set_handler(kKeyRight, kKeyEventPressed, app_operation_log_on_press_right_key);

	UIListPosIndicator* p = M_ui_list_pos_indicator(scr->view->get_child(kUI_IDListPosIndicator));
	p->set_max_value(record_log_size(kOperationLog));
	app_operation_log_view_log(scr, g_operation_log_view_index);
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
static void app_operation_log_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_operation_log_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_operation_log_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_operation_log_on_scr_activate(scr, scr->view);
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
void app_operation_log_enter(void)
{   
    screen_enter(kScreenOperationLog, app_operation_log_callback,NULL,
        g_operation_log_widgets, ssz_array_size(g_operation_log_widgets));
}
