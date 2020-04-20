/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-16 xqzhao
* Initial revision.
*
************************************************/
#include "app_select_popup.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_select_popup_widgets[] = {
  kWidgetAttrNull
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
static void app_select_popup_on_press_ok_key(KeyID key, int repeat_count)
{

}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void app_select_popup_on_press_back_key(KeyID key, int repeat_count)
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
static void app_select_popup_on_scr_create(Screen* scr)
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
static void app_select_popup_on_scr_widget_init(Screen* scr, UIView* scr_view)
{

}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_select_popup_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_select_popup_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_select_popup_on_press_back_key);
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
static void app_select_popup_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_select_popup_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_select_popup_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_select_popup_on_scr_activate(scr, scr->view);
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
void app_select_popup_enter(void)
{   
    screen_enter(kScreenSelectPopup, app_select_popup_callback,NULL,
        g_select_popup_widgets, ssz_array_size(g_select_popup_widgets));
}
