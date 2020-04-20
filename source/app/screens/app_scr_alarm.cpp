/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-05 xqzhao
* Initial revision.
*
************************************************/
#include "app_scr_alarm.h"
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
#include "alarm_play.h"
#include "param.h"
/************************************************
* Declaration
************************************************/
#define APP_SCR_ALARM_SHOW_TEXT_MAX_SIZE 40

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_scr_alarm_widgets[] = {
	kWidgetAttrBase,kUILable, kUI_IDLable, 0,0,-1,16,
		
	kWidgetAttrBase,kUILable, kUI_IDLable1, 0,16,-1,32,
	
	kWidgetAttrBase,kUIImage, kUI_IDImage2, 0,48,16,-1,
		kWidgetAttrImgID,kImgSoundPause,
	kWidgetAttrBase,kUILable, kUI_IDLable2, 16,48,80,-1,
		kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,
	
	kWidgetAttrBase,kUILable, kUI_IDLable3, -64,48,64,-1,
		kWidgetAttrAlign,UI_ALIGN_RIGHT | UI_ALIGN_VCENTER,
	
	kWidgetAttrBase,kUIImage, kUI_IDImage1, 24,16,36,32,

};
static AlarmID g_scr_alarm_show_id;
//static char g_scr_alarm_text[APP_SCR_ALARM_SHOW_TEXT_MAX_SIZE];
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
static void app_scr_alarm_on_press_ok_key(KeyID key, int repeat_count)
{
	AlarmLevel alarm_level = alarm_id_to_level(g_scr_alarm_show_id);
	if (alarm_level<kAlarmLevelHighest && 
		g_scr_alarm_show_id!=kAlarmBatteryExhaustionID) {
		alarm_clear(g_scr_alarm_show_id);
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
static void app_scr_alarm_on_press_back_key(KeyID key, int repeat_count)
{
	//UIView* p;
    //p = ui_view_focused();
	UILable* left_soft_key_lable = M_ui_lable(screen_current()->view->get_child(kUI_IDLable2));
	AlarmLevel alarm_level = alarm_id_to_level(g_scr_alarm_show_id);
	if ((alarm_level == kAlarmLevelHigh ||
		alarm_level == kAlarmLevelLowest ||
		alarm_level == kAlarmLevelLow ) &&
		!alarm_is_play_pause()) {
		if (g_scr_alarm_show_id != kAlarmBatteryExhaustionID) {
			alarm_pause_play();
			UIImage* pause_img = M_ui_image(screen_current()->view->get_child(kUI_IDImage2));
			left_soft_key_lable->set_text(get_string(kStrPauseAlarmingOk));
			left_soft_key_lable->set_pos(0, 48);
			pause_img->hide();
		}
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
static void app_scr_alarm_on_scr_create(Screen* scr)
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
static void app_scr_alarm_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	AlarmLevel alarm_level = alarm_id_to_level(g_scr_alarm_show_id);
	UILable* title_lable = M_ui_lable(scr_view->get_child(kUI_IDLable));
	title_lable->set_bk_color(UI_BLACK);
	title_lable->set_text_color(UI_WHITE);
	UILable* show_text_lable = M_ui_lable(scr_view->get_child(kUI_IDLable1));
	UILable* left_soft_key_lable = M_ui_lable(scr_view->get_child(kUI_IDLable2));
	UILable* right_soft_key_lable = M_ui_lable(scr_view->get_child(kUI_IDLable3));
	UIImage* img = M_ui_image(scr_view->get_child(kUI_IDImage1));
	UIImage* pause_img = M_ui_image(scr_view->get_child(kUI_IDImage2));
	const char* str = alarm_id_to_const_str(g_scr_alarm_show_id);

	char* show_text = get_dynamic_string(kStrDynamic1);
	if (alarm_level >= kAlarmLevelHighest) {
		snprintf(show_text, DYNAMIC_STRING_MAX_SIZE, "!!!%s %d", str, g_scr_alarm_show_id - kHighestAlarmStart);
	}else if(alarm_level>= kAlarmLevelHigh){
		snprintf(show_text, DYNAMIC_STRING_MAX_SIZE, "!!!%s", str);
	}else if(alarm_level>= kAlarmLevelLowest){
		snprintf(show_text, DYNAMIC_STRING_MAX_SIZE, "!%s", str);
	}else{
		show_text = (char*)str;
	}

	show_text_lable->set_text(show_text);

	pause_img->hide();
	StrID pause_str_id = kStrPauseAlarming;
	if(alarm_is_play_pause()){
		pause_str_id = kStrPauseAlarmingOk;
		left_soft_key_lable->set_pos(0, 48);
	} else {
		left_soft_key_lable->set_pos(pause_img->xsize(), 48);
	}

	switch (alarm_level) {
	case kAlarmLevelRemind:
	{
		title_lable->set_text("NOTICE");
	    right_soft_key_lable->set_text(get_string(kStrOk));
		img->hide();
	}
	break;
	case kAlarmLevelLowest:
	case kAlarmLevelLow:
	{
		title_lable->set_text("CAUTION");
		left_soft_key_lable->set_text(get_string(pause_str_id));
		if (pause_str_id == kStrPauseAlarming)
		{
			pause_img->show();
		}
		right_soft_key_lable->set_text(get_string(kStrCloseAlarming));
		img->set_img(get_image(kImgWarning));
	}
	break;
	case kAlarmLevelHigh:
	{
		title_lable->set_text("WARNING");
		if (g_scr_alarm_show_id != kAlarmBatteryExhaustionID) {
			right_soft_key_lable->set_text(get_string(kStrCloseAlarming));
			left_soft_key_lable->set_text(get_string(pause_str_id));
			if (pause_str_id == kStrPauseAlarming)
			{
				pause_img->show();
			}
		}
		img->set_img(get_image(kImgWarning));
	}
	break;

	case kAlarmLevelHighest:
	{
		title_lable->set_text("ERROR");		
//		screen_current()->view->set_bk_color(UI_WHITE);
//		show_text_lable->set_bk_color(UI_WHITE);
//		show_text_lable->set_text_color(UI_BLACK);
		img->set_img(get_image(kImgWarning));
		left_soft_key_lable->hide();
		right_soft_key_lable->hide();
	}
	break;



	default:
		ssz_assert_fail();
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
static void app_scr_alarm_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_alarm_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_alarm_on_press_back_key);

	//set title

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
static void app_scr_alarm_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_alarm_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_alarm_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_alarm_on_scr_activate(scr, scr->view);
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

void app_scr_alarm_resume_play_change_text(MsgParam param)
{
	if(!alarm_is_play_pause() && screen_current_id()==kScreenAlarm){
		//UIView* p;
	    //p = ui_view_focused();
		UILable* left_soft_key_lable = M_ui_lable(screen_current()->view->get_child(kUI_IDLable2));
		AlarmLevel alarm_level = alarm_id_to_level(g_scr_alarm_show_id);
		if (alarm_level == kAlarmLevelHigh ||
			alarm_level == kAlarmLevelLowest ||
			alarm_level == kAlarmLevelLow) {
			if (g_scr_alarm_show_id != kAlarmBatteryExhaustionID) {
				UIImage* pause_img = M_ui_image(screen_current()->view->get_child(kUI_IDImage2));
				left_soft_key_lable->set_text(get_string(kStrPauseAlarming));
				left_soft_key_lable->set_pos(pause_img->xsize(), 48);
				pause_img->show();
			}
		}
	}
}

void app_scr_alarm_enter(AlarmID alarm_id){
	g_scr_alarm_show_id = alarm_id;

	screen_enter_full_screen(kScreenAlarm, app_scr_alarm_callback,NULL,  
        g_scr_alarm_widgets, ssz_array_size(g_scr_alarm_widgets));
	msg_set_handler(kMsgAlarmPauseTimeOut, app_scr_alarm_resume_play_change_text);
}
