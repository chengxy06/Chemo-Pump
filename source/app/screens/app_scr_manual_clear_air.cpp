/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-25 dczhang
* Initial revision.
*
************************************************/
#include "app_scr_manual_clear_air.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "ui_image.h"
#include "ui_lable.h"
#include "app_delivery.h"
#include "record_log.h"
/************************************************
* Declaration
************************************************/
#define MANUAL_CLEAR_AIR_LABLE_Y ((DISPLAY_HEIGHT-MANUAL_CLEAR_AIR_LABLE_HEIGHT)/2-STATUSBAR_HEIGHT)
#define MANUAL_CLEAR_AIR_LABLE_HEIGHT 24

#define MANUAL_CLEAR_AIR_ICON_X_OFFSET 8
#define MANUAL_CLEAR_AIR_ICON_Y (MANUAL_CLEAR_AIR_LABLE_Y+8)

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_scr_manual_clear_air_widgets[] = {
	kWidgetAttrBase,kUILable, kUI_IDLable1, 
		0,MANUAL_CLEAR_AIR_LABLE_Y,SCREEN_WINDOW_WIDTH/2,MANUAL_CLEAR_AIR_LABLE_HEIGHT,
		kWidgetAttrTextID,kStrMedsBag1,
	kWidgetAttrBase,kUIImage, kUI_IDImage1,
		MANUAL_CLEAR_AIR_ICON_X_OFFSET,MANUAL_CLEAR_AIR_ICON_Y,8,8,

	kWidgetAttrBase,kUILable, kUI_IDLable2,
		SCREEN_WINDOW_WIDTH / 2,MANUAL_CLEAR_AIR_LABLE_Y,SCREEN_WINDOW_WIDTH / 2,MANUAL_CLEAR_AIR_LABLE_HEIGHT,
		kWidgetAttrTextID,kStrMedsBag2,
	kWidgetAttrBase,kUIImage, kUI_IDImage2,
		SCREEN_WINDOW_WIDTH / 2+MANUAL_CLEAR_AIR_ICON_X_OFFSET,MANUAL_CLEAR_AIR_ICON_Y,8,8,

    kWidgetAttrBase,kUILable, kUI_IDLable, 0,32,64,-1,
		kWidgetAttrTextID,kStrReturn, kWidgetAttrAlign,UI_ALIGN_LEFT | UI_ALIGN_VCENTER,
};

static InfusionBagMeds g_manual_clear_air_bag_meg;
/************************************************
* Function 
************************************************/
static UIImage* app_scr_manual_clear_air_get_show_img(UIView* scr_view) {
	if (g_manual_clear_air_bag_meg == kBagAMeds) {
		return M_ui_image(scr_view->get_child(kUI_IDImage1));
	}
	else {
		return M_ui_image(scr_view->get_child(kUI_IDImage2));
	}
}
static void app_scr_manual_clear_air_update_view(Screen* scr)
{
	UIView* scr_view = scr->view;

	//set lable and img
	UIImage* a_img_p = M_ui_image(scr_view->get_child(kUI_IDImage1));
	UIImage* b_img_p = M_ui_image(scr_view->get_child(kUI_IDImage2));
	UILable* a_lable_p = M_ui_lable(scr_view->get_child(kUI_IDLable1));
	UILable* b_lable_p = M_ui_lable(scr_view->get_child(kUI_IDLable2));
	UIImage* show_img_p;

	if (g_manual_clear_air_bag_meg == kBagAMeds) {
		a_lable_p->set_bk_color(UI_WHITE);
		a_lable_p->set_text_color(UI_BLACK);
		a_img_p->set_reverse(true);
		a_img_p->show();
		show_img_p = a_img_p;

		b_lable_p->set_bk_color(UI_BLACK);
		b_lable_p->set_text_color(UI_WHITE);
		b_img_p->hide();
	}
	else {
		a_lable_p->set_bk_color(UI_BLACK);
		a_lable_p->set_text_color(UI_WHITE);
		a_img_p->hide();

		b_lable_p->set_bk_color(UI_WHITE);
		b_lable_p->set_text_color(UI_BLACK);
		b_img_p->set_reverse(true);
		b_img_p->show();
		show_img_p = b_img_p;

	}


	//set title and process
	if (app_delivery_state(kClearAir) == kInfusionRunning) {
		screen_set_title(scr, get_string(kStrPressPauseToStopClearAir));
		show_img_p->set_img(get_image(kImgExhaustRun));
	}
	else {
		screen_set_title(scr, get_string(kStrPressStartToStartClearAir));
		show_img_p->set_img(get_image(kImgExhaustPause));
	}
}

static void app_scr_manual_clear_air_update_delivery_state(Screen* scr)
{
	UIImage* p = app_scr_manual_clear_air_get_show_img(scr->view);
	if (app_delivery_state(kClearAir) != kInfusionRunning) {
		screen_set_title(scr, get_string(kStrPressStartToStartClearAir));
		p->set_img(get_image(kImgExhaustPause));
	}
	else {
		screen_set_title(scr, get_string(kStrPressPauseToStopClearAir));
		p->set_img(get_image(kImgExhaustRun));
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
//static void app_scr_manual_clear_air_on_press_ok_key(KeyID key, int repeat_count)
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
static void app_scr_manual_clear_air_on_press_back_key(KeyID key, int repeat_count)
{
	app_delivery_stop(kClearAir);
    screen_go_back();
}

/***********************************************
* Description:
*   handle start key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...
* Return:
*
************************************************/
static void app_scr_manual_clear_air_on_press_start_key(KeyID key, int repeat_count)
{
	//UIImage* p = app_scr_manual_clear_air_get_show_img(screen_current_view());
	if (app_delivery_state(kClearAir) == kInfusionReady) {
		app_delivery_start(kClearAir);
	}
	else if (app_delivery_state(kClearAir) == kInfusionRunning) {
		app_delivery_pause(kClearAir);
		//screen_set_title(screen_current(), get_string(kStrPressStartToStartClearAir));
		//p->set_img(get_image(kImgExhaustPause));
	}
	else {
		app_delivery_resume(kClearAir);
		//screen_set_title(screen_current(), get_string(kStrPressPauseToStopClearAir));
		//p->set_img(get_image(kImgExhaustRun));
	}
}

static void app_scr_manual_clear_air_on_press_left_key(KeyID key, int repeat_count)
{
	if (g_manual_clear_air_bag_meg!= kBagAMeds && app_delivery_state(kClearAir)!=kInfusionRunning) {
		g_manual_clear_air_bag_meg = kBagAMeds;
		InfusionParam param;

		ssz_mem_zero(&param, sizeof(param));
		param.total_dose = -1;
		param.meds_b_total_dose = 0;
		app_delivery_set_infusion_param(kClearAir, &param, 0,0);
		app_scr_manual_clear_air_update_view(screen_current());
	}
}

static void app_scr_manual_clear_air_on_press_right_key(KeyID key, int repeat_count)
{
	if (g_manual_clear_air_bag_meg != kBagBMeds && app_delivery_state(kClearAir) != kInfusionRunning) {
		g_manual_clear_air_bag_meg = kBagBMeds;
		InfusionParam param;

		ssz_mem_zero(&param, sizeof(param));
		param.total_dose = 0;
		param.meds_b_total_dose = -1;
		app_delivery_set_infusion_param(kClearAir, &param,0,0);
		app_scr_manual_clear_air_update_view(screen_current());
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
static void app_scr_manual_clear_air_on_scr_create(Screen* scr)
{
	g_manual_clear_air_bag_meg = kBagAMeds;
	InfusionParam param;

	ssz_mem_zero(&param, sizeof(param));
	param.total_dose = -1;
	param.meds_b_total_dose = 0;
	app_delivery_set_infusion_param(kClearAir, &param, 0,0);

	record_log_add_info_with_data(kHistoryLog, kLogEventEnterManualClearAir, NULL, 0);
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
static void app_scr_manual_clear_air_on_scr_widget_init(Screen* scr, UIView* scr_view)
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
static void app_scr_manual_clear_air_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    //key_set_handler(kKeyOK, kKeyEventPressed, app_scr_manual_clear_air_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_manual_clear_air_on_press_back_key);
	key_set_handler(kKeyStart, kKeyEventPressed, app_scr_manual_clear_air_on_press_start_key);
	key_set_handler(kKeyLeft, kKeyEventPressed, app_scr_manual_clear_air_on_press_left_key);
	key_set_handler(kKeyRight, kKeyEventPressed, app_scr_manual_clear_air_on_press_right_key);

	app_scr_manual_clear_air_update_view(scr);
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
static void app_scr_manual_clear_air_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_manual_clear_air_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_manual_clear_air_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_manual_clear_air_on_scr_activate(scr, scr->view);
            break;
		case kUIMsgForceDrawAfterAllDraw:
			ui_set_pen_color(UI_WHITE);
			if (g_manual_clear_air_bag_meg==kBagAMeds) {
				ui_draw_rect_at(SCREEN_WINDOW_WIDTH / 2, MANUAL_CLEAR_AIR_LABLE_Y,
					SCREEN_WINDOW_WIDTH / 2, MANUAL_CLEAR_AIR_LABLE_HEIGHT, kUIDrawStroke);
			}
			else {
				ui_draw_rect_at(0, MANUAL_CLEAR_AIR_LABLE_Y,
					SCREEN_WINDOW_WIDTH / 2, MANUAL_CLEAR_AIR_LABLE_HEIGHT, kUIDrawStroke);
			}
			break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
		case kUIMsgDeliveryStateChanged:
			app_scr_manual_clear_air_update_delivery_state(scr);
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
void app_scr_manual_clear_air_enter(void)
{   
    screen_enter(kScreenScrManualClearAir, app_scr_manual_clear_air_callback,NULL,
        g_scr_manual_clear_air_widgets, ssz_array_size(g_scr_manual_clear_air_widgets));
}
