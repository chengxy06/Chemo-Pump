/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-26 xqzhao
* Initial revision.
*
************************************************/
#include "app_main_menu.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "image_data.h"
#include "ui_text_list.h"
#include "app_common.h"
#include "ui_image_list.h"
#include "app_history_log.h"
#include "ui_page_control.h"
#include "app_scr_set_total_dose.h"
#include "app_popup.h"
#include "app_alternative_popup.h"
#include "app_scr_auto_clear_air.h"
#include "app_scr_manual_clear_air.h"
#include<string.h>  
#include<stdio.h>  
#include<stdlib.h>  
#include "mid_common.h"
#include "app_set_menu.h"
#include "scr_clean_tube.h"
#include "app_delivery.h"
#include "scr_password_popup.h"
#include "data.h"
#include "param.h"

/************************************************
* Declaration
************************************************/
#define MAIN_MENU_ICON_XSIZE 64
#define MAIN_MENU_ICON_YSIZE 64

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_main_menu_widgets[] = {
	kWidgetAttrBase,kUIImageList, kUI_IDImageList, 0,0,MAIN_MENU_ICON_XSIZE,MAIN_MENU_ICON_YSIZE,
		kWidgetAttrAlign,UI_ALIGN_CENTER,//image list

	kWidgetAttrBase,kUIStatusBar, kUI_IDStatusBar, MAIN_MENU_ICON_XSIZE,0,-1,14,//status bar

	kWidgetAttrBase,kUIPageControl, kUI_IDPageControl,
		MAIN_MENU_ICON_XSIZE,48,SCREEN_WIDTH - MAIN_MENU_ICON_XSIZE * 2,11,//page control
	
	kWidgetAttrBase,kUITextList, kUI_IDTextList,
		MAIN_MENU_ICON_XSIZE,16,SCREEN_WIDTH - MAIN_MENU_ICON_XSIZE * 2,32,
		kWidgetAttrAlign,UI_ALIGN_CENTER,//text list

	kWidgetAttrBase,kUILable, kUI_IDLable, -64,-16,-1,-1,
		kWidgetAttrTextID,kStrEnter,kWidgetAttrAlign,UI_ALIGN_RIGHT|UI_ALIGN_HCENTER//right bottom lable
};


static const StrImgIDItem g_main_menu_items[] = {
 	{ kStrClearAir,kImgExhaust },   
	{ kStrNewInfusionTask,kImgNewTask },
	{ kStrStopInfusion,kImgStopInfusion },
	{ kStrSetting,kImgSystemSetting },
	{kStrHistoryLog,kImgHistoryLog},
};
/************************************************
* Function 
************************************************/


/***********************************************
* Description:
*	handle alternative popup msg
* Argument:
*	ret_code,select_index
*
* Return:
*
************************************************/
static void app_main_menu_on_select_clear_air_mode(PopupReturnCode ret_code, int select_index)
{
	switch (ret_code) {
	case kPopupReturnOK:
	{
		if (0 == select_index) {
			InfusionParam param;

			ssz_mem_zero(&param, sizeof(param));
			param.total_dose = AUTO_CLEAR_AIR_MEDS_A_TOTAL_DOSE_uL;
			param.meds_b_total_dose = AUTO_CLEAR_AIR_MEDS_B_TOTAL_DOSE_uL;
			app_delivery_set_infusion_param(kClearAir, &param, 0,0);
			app_scr_auto_clear_air_enter();
		}
		else if (1 == select_index) {
			app_scr_manual_clear_air_enter();
		}
	}
	break;

	case kPopupReturnCancel:

		break;
	}
}
static void app_main_menu_on_select_if_stop_infuse(PopupReturnCode ret_code, int select_index) {
	switch (ret_code) {
	case kPopupReturnOK:
	{
		if (0 == select_index) {
			app_delivery_stop(kNormalInfusion);
		}
		else if (1 == select_index) {
		}
	}
	break;

	case kPopupReturnCancel:

		break;
	}
}

static void app_main_menu_on_select_if_stop_infuse_by_clean_tube(PopupReturnCode ret_code, int select_index) {
	switch (ret_code) {
	case kPopupReturnOK:
	{
		if (0 == select_index) {
			app_delivery_stop(kNormalInfusion);
			InfusionParam param;
			ssz_mem_zero(&param, sizeof(param));
			param.meds_b_total_dose = app_delivery_info(kNormalInfusion)->meds_b_total_dose - app_delivery_info(kNormalInfusion)->meds_b_infused_dose;
			if (param.meds_b_total_dose <0) {
				param.meds_b_total_dose = 0;
			}
			app_delivery_start_with_param(kCleanTube, &param);
			scr_clean_tube_enter();
		}
		else if (1 == select_index) {
		}
	}
	break;

	case kPopupReturnCancel:

		break;
	}
}
static void app_main_menu_on_select_stop_infuse_way(PopupReturnCode ret_code, int select_index)
{
	switch (ret_code) {
	case kPopupReturnOK:
	{
		if (0 == select_index) {
			app_alternative_popup_enter(get_string(kStrWhetherToStopInfuse),
				get_string(kStrYes), get_string(kStrNo),
				0, app_main_menu_on_select_if_stop_infuse);

		}
		else if (1 == select_index) {
			app_alternative_popup_enter(get_string(kStrWhetherToStopInfuseAndClean),
				get_string(kStrYes), get_string(kStrNo),
				0, app_main_menu_on_select_if_stop_infuse_by_clean_tube);
		}
	}
	break;

	case kPopupReturnCancel:

		break;
	}
}


/***********************************************
* Description:
*	handle popup msg
* Argument:
*	ret_code:
*
* Return:
*
************************************************/
static void app_main_menu_on_clear_air_remind_confirm(PopupReturnCode ret_code)
{
	switch(ret_code){
		case kPopupReturnOK:
		{
			//const char *s_title = get_string(kStrSelectClearAirMode);
			//const char *s_select1 = get_string(kStrAutoMode);
			//const char *s_select2 = get_string(kStrManualMode);

			//app_alternative_popup_enter(s_title, s_select1, s_select2, 0, app_main_menu_on_select_clear_air_mode);
			app_scr_manual_clear_air_enter();
		}
			break;

		case kPopupReturnCancel:

			break;
	}
}

static void app_main_menu_on_enter_new_task(PopupReturnCode ret_code)
{
	switch(ret_code){
		case kPopupReturnOK:
		{
			app_scr_set_total_dose_enter();;
		}
			break;

		case kPopupReturnWrongPassword:
			break;
	}
}

static void app_main_menu_on_enter_clear_air(PopupReturnCode ret_code)
{
	switch(ret_code){
		case kPopupReturnOK:
		{
			const char *s = get_string(kStrDoNotConnectBody);
		    app_popup_enter(kPopupInformation, NULL, s, app_main_menu_on_clear_air_remind_confirm);
		}
			break;

		case kPopupReturnWrongPassword:
			break;
	}
}

static void app_main_menu_on_enter_stop_infusion(PopupReturnCode ret_code)
{
	switch(ret_code){
		case kPopupReturnOK:
		{
            if (app_delivery_info(kNormalInfusion)->meds_b_infused_dose < app_delivery_info(kNormalInfusion)->meds_b_total_dose) {
                const char *s_title = get_string(kStrSelectFinishMode);
                const char *s_select1 = get_string(kStrDirectlyStopInfuse);
                const char *s_select2 = get_string(kStrStopByCleanTube);
            
                app_alternative_popup_enter(s_title, s_select1, s_select2, 0, app_main_menu_on_select_stop_infuse_way);
            }
            else {
                app_alternative_popup_enter(get_string(kStrWhetherToStopInfuse),
                    get_string(kStrYes), get_string(kStrNo),
                    0, app_main_menu_on_select_if_stop_infuse);
            }
		}
			break;

		case kPopupReturnWrongPassword:
			break;
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
static void app_main_menu_on_press_ok_key(KeyID key, int repeat_count)
{
	UITextList* p = (UITextList*)screen_current_view()->get_child(kUI_IDTextList);
	switch (g_main_menu_items[p->get_selected()].str_id) {
	case kStrNewInfusionTask:
		ssz_mem_zero(&g_user_set_infusion_para, sizeof(g_user_set_infusion_para));
		g_user_set_infusion_para.limit_infusion_speed = MAX_INFUSION_SPEED/100;
        g_user_set_infusion_para.limit_infusion_duration = MAX_INFUSION_DURATION;

		if (app_delivery_state(kNormalInfusion) != kInfusionStop) {
			g_user_set_infusion_para.total_dose = (app_delivery_info(kNormalInfusion)->total_dose-app_delivery_info(kNormalInfusion)->infused_dose)/1000;
			if (g_user_set_infusion_para.total_dose < 0) {
				g_user_set_infusion_para.total_dose = 0;
			}
			//g_user_set_infusion_para.first_infusion_dose = app_delivery_info(kNormalInfusion)->first_infusion_dose;
			g_user_set_infusion_para.meds_b_total_dose = (app_delivery_info(kNormalInfusion)->meds_b_total_dose-app_delivery_info(kNormalInfusion)->meds_b_infused_dose)/1000;
			if (g_user_set_infusion_para.meds_b_total_dose < 0) {
				g_user_set_infusion_para.meds_b_total_dose = 0;
			}
			g_user_set_infusion_para.infusion_speed = app_delivery_info(kNormalInfusion)->infusion_speed/100;
			g_user_set_infusion_para.is_peek_infusion_mode = app_delivery_info(kNormalInfusion)->is_peek_infusion_mode;
			g_user_set_infusion_para.peek_infusion_start_time_point = app_delivery_info(kNormalInfusion)->peek_infusion_start_time_point;
			g_user_set_infusion_para.peek_infusion_end_time_point = app_delivery_info(kNormalInfusion)->peek_infusion_end_time_point;
			g_user_set_infusion_para.peek_infusion_speed = app_delivery_info(kNormalInfusion)->peek_infusion_speed/100;
		}
		else{
			g_user_set_infusion_para.total_dose = 50;
			//g_user_set_infusion_para.meds_b_total_dose = 10; //TODO
			UserSetInfusionParaToSave info;
			if(data_read(kDataSetInfusionParaInfoBlock, &info, sizeof(info))==kSuccess){
				g_user_set_infusion_para.total_dose = info.total_dose;
				g_user_set_infusion_para.meds_b_total_dose = info.meds_b_total_dose;
				g_user_set_infusion_para.infusion_speed = info.infusion_speed;
				g_user_set_infusion_para.infusion_duration = info.infusion_duration;
				g_user_set_infusion_para.peek_infusion_start_time_point = info.peek_infusion_start_time_point;
				g_user_set_infusion_para.peek_infusion_end_time_point = info.peek_infusion_end_time_point;
				g_user_set_infusion_para.peek_infusion_speed = info.peek_infusion_speed;
			}
		}

		if (app_delivery_state(kNormalInfusion) == kInfusionPause ||
			app_delivery_state(kNormalInfusion) == kInfusionRunning) {
			g_factory_password = data_read_int(kDataPassword);
			scr_password_popup_enter(&g_factory_password, NULL, app_main_menu_on_enter_new_task);
		}
		else {
			app_scr_set_total_dose_enter();
		}

		break;
	case kStrClearAir:
		//app_scr_clear_air_enter();
	{
        if (app_delivery_state(kNormalInfusion) == kInfusionPause||
			app_delivery_state(kNormalInfusion) == kInfusionRunning) {
            g_factory_password=data_read_int(kDataPassword);
            scr_password_popup_enter(&g_factory_password, NULL, app_main_menu_on_enter_clear_air);         
        
        }else{
            const char *s = get_string(kStrDoNotConnectBody);
		    app_popup_enter(kPopupInformation, NULL, s, app_main_menu_on_clear_air_remind_confirm);
        }
	}
		break;
	case kStrStopInfusion:
		if (app_delivery_state(kNormalInfusion) == kInfusionPause ||
			app_delivery_state(kNormalInfusion) == kInfusionRunning) {
            g_factory_password=data_read_int(kDataPassword);
            scr_password_popup_enter(&g_factory_password, NULL, app_main_menu_on_enter_stop_infusion);
        }else if(app_delivery_state(kNormalInfusion) == kInfusionReady){
			app_alternative_popup_enter(get_string(kStrWhetherToStopInfuse),
				get_string(kStrYes), get_string(kStrNo),
				0, app_main_menu_on_select_if_stop_infuse);
        }
		break;
	case kStrSetting:
		app_set_menu_enter();
		break;
	case kStrHistoryLog:
		app_history_log_enter();
		break;
	default:
		break;
	}
}

/***********************************************
* Description:
*   handle back key
* Argument:
*
* Return:
*
************************************************/
static void app_main_menu_on_press_back_key(KeyID key, int repeat_count)
{
	if (app_delivery_state(kNormalInfusion)!=kInfusionStop) {
		screen_go_back();
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
static void app_main_menu_on_scr_create(Screen* scr)
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
static void app_main_menu_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	//add item
	UITextList* p = (UITextList*)scr_view->get_child(kUI_IDTextList);
	UIImageList* p_img_list = (UIImageList*)scr_view->get_child(kUI_IDImageList);

	for (int i = 0; i < ssz_array_size(g_main_menu_items); i++) {
		p->push_back(get_string((g_main_menu_items[i].str_id)));
		p_img_list->push_back(get_image(g_main_menu_items[i].img_id), get_image(g_main_menu_items[i].img_id));
	}

	UIPageControl* p_page_control= (UIPageControl*)scr_view->get_child(kUI_IDPageControl);
	p_page_control->set_size(ssz_array_size(g_main_menu_items));
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_main_menu_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_main_menu_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_main_menu_on_press_back_key);

	if (app_delivery_state(kNormalInfusion) != kInfusionStop) {
		UITextList* p = (UITextList*)scr_view->get_child(kUI_IDTextList);
		p->set_text(1, get_string(kStrEditInfusionTask));
	}
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
static void app_main_menu_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_main_menu_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_main_menu_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_main_menu_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
		case kUIMsgNotifyParent:
			if (msg->int_param==kUINotifySelectChanged &&
				msg->src->id()==kUI_IDTextList){
				UITextList* p = (UITextList*)scr->view->get_child(kUI_IDTextList);
				UIImageList* p_img_list = (UIImageList*)scr->view->get_child(kUI_IDImageList);
				p_img_list->select(p->get_selected());
				UIPageControl* p_page_control = (UIPageControl*)scr->view->get_child(kUI_IDPageControl);
				p_page_control->select(p->get_selected());
			}
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
void app_main_menu_enter(void)
{   
    screen_enter_full_screen(kScreenMainMenu, app_main_menu_callback,NULL,
        g_main_menu_widgets, ssz_array_size(g_main_menu_widgets));
}
