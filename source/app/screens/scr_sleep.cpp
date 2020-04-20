/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-10-27 xqzhao
* Initial revision.
*
************************************************/
#include "scr_sleep.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "display.h"

/************************************************
* Declaration
************************************************/
//each time to check if can sleep
#define TRY_TO_SLEEP_MS_AT_SLEEP_SCREEN 20

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_sleep_widgets[] = {
  kWidgetAttrNull
};
static bool g_sleep_is_can_enter;
/************************************************
* Function 
************************************************/

static void scr_sleep_try_to_sleep() {
	display_close();
	if (!is_sleep_allow()) {
		g_sleep_is_can_enter = false;
		timer_start_periodic_every(kTimerScreen, TRY_TO_SLEEP_MS_AT_SLEEP_SCREEN);
	}else if(!g_sleep_is_can_enter){
		//can enter, delay 5ms to enter sleep
		g_sleep_is_can_enter = true;
		timer_start_periodic_every(kTimerScreen, 5);
	}
	else {
//#ifdef SSZ_TARGET_SIMULATOR
		msg_post_int(kMsgGoToSleep, 0);
//#endif
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
static void scr_sleep_on_press_ok_key(KeyID key, int repeat_count)
{
	screen_go_back();
}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
//static void scr_sleep_on_press_back_key(KeyID key, int repeat_count)
//{
//    screen_go_back();
//}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void scr_sleep_on_scr_create(Screen* scr)
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
static void scr_sleep_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
    //scr_layout_load_standard(scr, NULL, get_string(kStrReturn), NULL);
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void scr_sleep_on_scr_activate(Screen* scr, UIView* scr_view)
{
	key_clear_all_handler();
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, scr_sleep_on_press_ok_key);
    //key_set_handler(kKeyBack, kKeyEventPressed, scr_sleep_on_press_back_key);

	screen_set_timer_handler(kTimerScreen, scr_sleep_try_to_sleep);
	timer_start_periodic_every(kTimerScreen, TRY_TO_SLEEP_MS_AT_SLEEP_SCREEN);
	g_sleep_is_can_enter = false;
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
static void scr_sleep_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            scr_sleep_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			scr_sleep_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            scr_sleep_on_scr_activate(scr, scr->view);
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
void scr_sleep_enter(void)
{   
	Rect screen_rect;
	ui_rect_init_by_size(screen_rect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    screen_enter_ex(kScreenSleep, scr_sleep_callback,NULL,
		&screen_rect, g_sleep_widgets, ssz_array_size(g_sleep_widgets), SCREEN_MODE_AUTO_DELETE_WHEN_DEACTIVE);
}
