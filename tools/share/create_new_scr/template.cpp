/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 {date} {author}
* Initial revision.
*
************************************************/
#include "{name}.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_{no_prefix_name}_widgets[] = {
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
static void {name}_on_press_ok_key(KeyID key, int repeat_count)
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
static void {name}_on_press_back_key(KeyID key, int repeat_count)
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
static void {name}_on_scr_create(Screen* scr)
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
static void {name}_on_scr_widget_init(Screen* scr, UIView* scr_view)
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
static void {name}_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, {name}_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, {name}_on_press_back_key);
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
static void {name}_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            {name}_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			{name}_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            {name}_on_scr_activate(scr, scr->view);
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
void {name}_enter(void)
{   
    screen_enter(kScreen{no_suffix_prefix_camelcase_name}, {name}_callback,NULL,
        g_{no_prefix_name}_widgets, ssz_array_size(g_{no_prefix_name}_widgets));
}
