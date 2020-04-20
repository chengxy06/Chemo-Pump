/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-25 dczhang
* Initial revision.
*
************************************************/
#include "app_scr_auto_clear_air.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "ui_image.h"
#include "ui_progress_bar.h"
#include "scr_layout.h"
#include "app_delivery.h"
#include "record_log.h"
/************************************************
* Declaration
************************************************/
#define AUTO_CLEAR_UPDATE_PROGRESS_TIMER kTimerScreen
#define AUTO_CLEAR_UPDATE_PROGRESS_TIMER_MS 50

/************************************************
* Variable 
************************************************/
//widgets at this screen
//static const int16_t g_scr_auto_clear_air_widgets[] = {
//	kWidgetAttrBase,kUIImage, kUI_IDImage,
//		RUN_INDICATOR_X,RUN_INDICATOR_Y- STATUSBAR_HEIGHT,RUN_INDICATOR_WIDTH,RUN_INDICATOR_HEIGHT,
//	kWidgetAttrBase,kUIProgressBar, kUI_IDProgressBar,
//		PROGRESS_BAR_X,PROGRESS_BAR_Y - STATUSBAR_HEIGHT,PROGRESS_BAR_WIDTH,PROGRESS_BAR_HEIGHT,
//    kWidgetAttrBase,kUILable, kUI_IDLable1, 0,32,64,-1,
//		kWidgetAttrTextID,kStrReturn, kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,
//};

/************************************************
* Function 
************************************************/

static void app_scr_auto_clear_air_update_view(Screen* scr)
{
	UIView* scr_view = scr->view;
	//set title and process
	UIImage* img_p = M_ui_image(scr_view->get_child(kUI_IDImage));
	UIProgressBar* progress_bar_p = M_ui_progress_bar(scr_view->get_child(kUI_IDProgressBar));
	if (app_delivery_state(kClearAir) == kInfusionRunning) {
		screen_set_title(scr, get_string(kStrPressPauseToStopClearAir));
		img_p->set_img(get_image(kImgExhaustRun));
	}
	else {
		screen_set_title(scr, get_string(kStrPressStartToStartClearAir));
		img_p->set_img(get_image(kImgExhaustPause));
	}
	progress_bar_p->set_value(app_delivery_infused_dose(kClearAir) + app_delivery_meds_b_infused_dose(kClearAir));

}

/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
//static void app_scr_auto_clear_air_on_press_ok_key(KeyID key, int repeat_count)
//{
//
//}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void app_scr_auto_clear_air_on_press_back_key(KeyID key, int repeat_count)
{
	app_delivery_stop(kClearAir);
    screen_go_back();
}

static void app_scr_auto_clear_air_on_press_start_key(KeyID key, int repeat_count)
{
	if (app_delivery_state(kClearAir) == kInfusionReady) {
		app_delivery_start(kClearAir);
	}
	else if (app_delivery_state(kClearAir)==kInfusionRunning) {
		app_delivery_pause(kClearAir);
	}
	else {
		app_delivery_resume(kClearAir);
	}
	//app_scr_auto_clear_air_update_view(screen_current());
}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void app_scr_auto_clear_air_on_scr_create(Screen* scr)
{
	record_log_add_info_with_data(kHistoryLog, kLogEventEnterAutoClearAir, NULL, 0);
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
static void app_scr_auto_clear_air_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	scr_layout_load_progress(scr, NULL, get_string(kStrReturn), NULL);
	UIProgressBar* progress_bar_p = M_ui_progress_bar(scr_view->get_child(kUI_IDProgressBar));
	progress_bar_p->set_max_value(app_delivery_info(kClearAir)->meds_b_total_dose+
		app_delivery_info(kClearAir)->total_dose);
}
static void app_scr_auto_clear_air_update_progress() {
	UIProgressBar* progress_bar_p = M_ui_progress_bar(screen_current_view()->get_child(kUI_IDProgressBar));

	progress_bar_p->set_value(app_delivery_infused_dose(kClearAir) + app_delivery_meds_b_infused_dose(kClearAir));
}
/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_scr_auto_clear_air_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    //key_set_handler(kKeyOK, kKeyEventPressed, app_scr_auto_clear_air_on_press_ok_key);
	key_set_handler(kKeyBack, kKeyEventPressed, app_scr_auto_clear_air_on_press_back_key);
	key_set_handler(kKeyStart, kKeyEventPressed, app_scr_auto_clear_air_on_press_start_key);

	//start time to update progressbar
	screen_set_timer_handler(AUTO_CLEAR_UPDATE_PROGRESS_TIMER, app_scr_auto_clear_air_update_progress);
	timer_start_periodic_every(AUTO_CLEAR_UPDATE_PROGRESS_TIMER, AUTO_CLEAR_UPDATE_PROGRESS_TIMER_MS);
	//update view
	app_scr_auto_clear_air_update_view(scr);

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
static void app_scr_auto_clear_air_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_auto_clear_air_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_auto_clear_air_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_auto_clear_air_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
		case kUIMsgDeliveryStateChanged:
			app_scr_auto_clear_air_update_view(scr);
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
void app_scr_auto_clear_air_enter(void)
{   
    screen_enter(kScreenScrAutoClearAir, app_scr_auto_clear_air_callback,NULL,
        NULL, 0);
}
