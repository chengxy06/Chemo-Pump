/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-21 xqzhao
* Initial revision.
*
************************************************/
#include "scr_clean_tube.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "app_delivery.h"
#include "ui_image.h"
#include "ui_progress_bar.h"
#include "scr_layout.h"
#include "app_alternative_popup.h"

/************************************************
* Declaration
************************************************/
#define CLEAN_TUBE_UPDATE_PROGRESS_TIMER kTimerScreen
#define CLEAN_TUBE_UPDATE_PROGRESS_TIMER_MS 50

/************************************************
* Variable 
************************************************/
//widgets at this screen
//static const int16_t g_clean_tube_widgets[] = {
//  kWidgetAttrNull
//};

/************************************************
* Function 
************************************************/

static void scr_clean_tube_update_view(Screen* scr)
{
	UIView* scr_view = scr->view;
	//set title and process
	UIImage* img_p = M_ui_image(scr_view->get_child(kUI_IDImage));
	UIProgressBar* progress_bar_p = M_ui_progress_bar(scr_view->get_child(kUI_IDProgressBar));
	if (app_delivery_state(kCleanTube) == kInfusionRunning) {
		screen_set_title(scr, get_string(kStrPressPauseToStopCleanTube));
		img_p->set_img(get_image(kImgExhaustRun));
	}
	else {
		screen_set_title(scr, get_string(kStrPressStartToStartCleanTube));
		img_p->set_img(get_image(kImgExhaustPause));
	}
	progress_bar_p->set_value(app_delivery_meds_b_infused_dose(kCleanTube));

}

/***********************************************
* Description:
*   handle ok key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
//static void scr_clean_tube_on_press_ok_key(KeyID key, int repeat_count)
//{
//
//}

static void scr_clean_tube_on_select_if_stop_clean(PopupReturnCode ret_code, int select_index) {
	switch (ret_code) {
	case kPopupReturnOK:
	{
		if (0 == select_index) {
			app_delivery_stop(kCleanTube);
			screen_delete_by_id(kScreenCleanTube);
		}
		else if (1 == select_index) {
		}
	}
	break;

	case kPopupReturnCancel:

		break;
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
static void scr_clean_tube_on_press_back_key(KeyID key, int repeat_count)
{
	app_alternative_popup_enter(get_string(kStrWhetherToStopClean),
		get_string(kStrYes), get_string(kStrNo),
		0, scr_clean_tube_on_select_if_stop_clean);
}

static void scr_clean_tube_on_press_start_key(KeyID key, int repeat_count)
{
	if (app_delivery_state(kCleanTube)==kInfusionRunning) {
		app_delivery_pause(kCleanTube);
	}
	else {
		app_delivery_resume(kCleanTube);
	}
	//scr_clean_tube_update_view(screen_current());
}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void scr_clean_tube_on_scr_create(Screen* scr)
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
static void scr_clean_tube_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	scr_layout_load_progress(scr, NULL, get_string(kStrStop), NULL);
	UIProgressBar* progress_bar_p = M_ui_progress_bar(scr_view->get_child(kUI_IDProgressBar));
	progress_bar_p->set_max_value(app_delivery_info(kCleanTube)->meds_b_total_dose);
}
static void scr_clean_tube_update_progress() {
	UIProgressBar* progress_bar_p = M_ui_progress_bar(screen_current_view()->get_child(kUI_IDProgressBar));

	progress_bar_p->set_value(app_delivery_meds_b_infused_dose(kCleanTube));
}
/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_clean_tube_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    //key_set_handler(kKeyOK, kKeyEventPressed, scr_clean_tube_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, scr_clean_tube_on_press_back_key);
	key_set_handler(kKeyStart, kKeyEventPressed, scr_clean_tube_on_press_start_key);

	//start time to update progressbar
	screen_set_timer_handler(CLEAN_TUBE_UPDATE_PROGRESS_TIMER, scr_clean_tube_update_progress);
	timer_start_periodic_every(CLEAN_TUBE_UPDATE_PROGRESS_TIMER, CLEAN_TUBE_UPDATE_PROGRESS_TIMER_MS);
	//update view
	scr_clean_tube_update_view(scr);

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
static void scr_clean_tube_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_clean_tube_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_clean_tube_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_clean_tube_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
		case kUIMsgDeliveryStateChanged:
			scr_clean_tube_update_view(scr);
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
void scr_clean_tube_enter(void)
{   
    screen_enter(kScreenCleanTube, scr_clean_tube_callback,NULL,
        NULL, 0);
}
