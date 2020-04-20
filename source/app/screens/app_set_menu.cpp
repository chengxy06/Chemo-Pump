/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-09-20 jcwu
* Initial revision.
*
************************************************/
#include "app_set_menu.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "scr_layout.h"
#include "ui_image.h"
#include "scr_layout.h"
#include "ui_lable.h"
#include "ui_image_list.h"
#include "ui_text_list.h"
#include "app_scr_set_time.h"
#include "version.h"
#include "string_data.h"
#include "app_popup.h"
#include "app_alternative_popup.h"
#include "data.h"
#include "app_scr_set_max_clean_tube_dose.h"
#include "record_log.h"


/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

#define UI_ALIGN_CENTER_HCENTER (UI_ALIGN_HCENTER|UI_TEXT_ALIGN_HCENTER) //center at horizontal and vertical 

static const int16_t g_set_menu_widgets[] = {
	kWidgetAttrBase,kUIImageList, kUI_IDImageList,28,5,200,40,
	kWidgetAttrAlign,UI_ALIGN_CENTER_HCENTER,//image list
};

static const StrWithTwoImgIDItem g_set_menu_items[] = {
 	{ kStrTimeDate,kImgTimeUnselect, kImgTimeSelect },
	{ kStrVersion,kImgInfoUnselect, kImgInfoSelect  },
	{ kStrPeekModeSwitch,kImgPeakSetUnselect,kImgPeakSetSelect},
	{ kStrInfusionModeSetting,kImgModeUnselected,kImgModeSelected},
    { kStrSetMaxCleanTubeDose,kImgFlushMaxDoseUnselected, kImgFlushMaxDoseSelected },
};

/************************************************
* Function 
************************************************/

void app_set_menu_on_select_peak_mode(PopupReturnCode ret_code, int select_index) {
    LogVar log_var;
    log_var.type = kLogVarString;
	bool is_enable_peek_mode;

    if (ret_code==kPopupReturnOK) {
		if (select_index == 1) {
			is_enable_peek_mode = true;
            log_var.str_value = "on";
		}
		else {
			is_enable_peek_mode = false;
			log_var.str_value = "off";
		}
		data_write_int(kDataPeekModeSwitch, is_enable_peek_mode);
        record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
                       kLogEventSetPeakMode, &log_var, 1);
	}
}

void app_set_menu_on_select_infusion_mode_select(PopupReturnCode ret_code, int select_index) {
    LogVar log_var;
    log_var.type = kLogVarString;

    if (ret_code==kPopupReturnOK) {
		InfusionMode infusion_mode;
		if (select_index == 1) {
			infusion_mode = kSetTimeMode;
            log_var.str_value = "duration-mode";
		}
		else {
			infusion_mode = kSetSpeedMode;
            log_var.str_value = "speed-mode";
		}
		data_write_int(kDataInfusionModeSelection, infusion_mode);
        record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
                       kLogEventSetInfusionMode, &log_var, 1);
	}
}

void app_set_menu_on_select_infusion_mode(PopupReturnCode ret_code, int select_index) {
    LogVar log_var;
    log_var.type = kLogVarString;

    if (ret_code==kPopupReturnOK) {
        InfusionMode infusion_mode;
		if (select_index == 1) {
            InfusionMode infusion_mode_select = (InfusionMode)data_read_int(kDataInfusionModeSelection);
            app_alternative_popup_enter(get_string(kStrModeSelection), get_string(kStrSetSpeedMode), get_string(kStrSetTimeMode),infusion_mode_select-1, app_set_menu_on_select_infusion_mode_select);
		}
		else {
			infusion_mode = kAskEverytime;
            data_write_int(kDataInfusionModeSelection, infusion_mode);
            log_var.str_value = "always-ask-mode";
            record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
                           kLogEventSetInfusionMode, &log_var, 1);
		}
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
static void app_set_menu_on_press_ok_key(KeyID key, int repeat_count)
{
	UIImageList* p = (UIImageList*)screen_current_view()->get_child(kUI_IDImageList);
	switch (g_set_menu_items[p->get_selected()].str_id) {
 	case kStrTimeDate:
		app_scr_set_time_enter();
		break;
	case kStrVersion:
	    {
		    char *tmp = get_dynamic_string(kStrDynamic1);
		    snprintf(tmp, DYNAMIC_STRING_MAX_SIZE, "V%d.%d", version_info()->major, version_info()->minor);
		    app_popup_enter(kPopupInformation, NULL, tmp, NULL);
	    }
	    break;
    case kStrPeekModeSwitch:
	    {
		    bool is_enable_peek_mode = data_read_int(kDataPeekModeSwitch);
		    app_alternative_popup_enter(get_string(kStrPeekModeSwitch), get_string(kStrOff), get_string(kStrOn), is_enable_peek_mode, app_set_menu_on_select_peak_mode);
	    }
	    break;
    case kStrInfusionModeSetting:
	    {
		    InfusionMode infusion_mode_select = (InfusionMode)data_read_int(kDataInfusionModeSelection);
            if(infusion_mode_select >kAskEverytime) infusion_mode_select=(InfusionMode)1;
		    app_alternative_popup_enter(get_string(kStrInfusionModeSetting), get_string(kStrAskEverytimeMode), get_string(kStrFixMode), infusion_mode_select, app_set_menu_on_select_infusion_mode);
	    }
	break; 
    case kStrSetMaxCleanTubeDose:
	    {
            app_scr_set_max_clean_tube_dose_enter();
        }
	    break;
	default:
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
static void app_set_menu_on_press_back_key(KeyID key, int repeat_count)
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
static void app_set_menu_on_scr_create(Screen* scr)
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
static void app_set_menu_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	scr_layout_load_standard(scr, get_string(kStrPressureCalibration), get_string(kStrReturn), get_string(kStrEnter));

	//add item   
	UITextList* p = (UITextList*)scr_view->get_child(kUI_IDTextList);
	UIImageList* p_img_list = (UIImageList*)scr_view->get_child(kUI_IDImageList);
	p_img_list->set_layout_orientation(kUIHorizontal);
	p_img_list->set_item_width(40);

	for (int i = 0; i < ssz_array_size(g_set_menu_items); i++) {
		//p->push_back(get_string((g_set_menu_items[i].str_id)));
		p_img_list->push_back(get_image(g_set_menu_items[i].img_id), get_image(g_set_menu_items[i].selected_img_id));
	}

	UILable*  q = M_ui_lable(scr->view->get_child(kUI_IDLeftSoftKey));
	q->set_size(28, LEFT_SOFT_KEY_HEIGHT);
	UILable*  d = M_ui_lable(scr->view->get_child(kUI_IDRightSoftKey));
	d->set_size(28, RIGHT_SOFT_KEY_HEIGHT);
	d->set_pos( 228,34);
}

/***********************************************
* Description:
*   handle screen active msg, at this you can set title, bind key, set timer...
* Argument:
*
* Return:
*
************************************************/
static void app_set_menu_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_set_menu_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_set_menu_on_press_back_key);	
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
static void app_set_menu_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_set_menu_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_set_menu_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_set_menu_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
		case kUIMsgNotifyParent:
			if (msg->int_param == kUINotifySelectChanged &&
				msg->src->id() == kUI_IDImageList) {
				UIImageList* p_img_list  = (UIImageList*)scr->view->get_child(kUI_IDImageList);
				 
				if (g_set_menu_items[p_img_list->get_selected()].selected_img_id == kImgTimeSelect) {
					screen_set_title(scr, get_string(kStrTimeDate));
				}
				else if (g_set_menu_items[p_img_list->get_selected()].selected_img_id == kImgInfoSelect) {
					screen_set_title(scr, get_string(kStrVersion));
				}
                else if (g_set_menu_items[p_img_list->get_selected()].selected_img_id == kImgPeakSetSelect) {
					screen_set_title(scr, get_string(kStrPeekModeSwitch));
				} else if (g_set_menu_items[p_img_list->get_selected()].selected_img_id == kImgModeSelected) {
					screen_set_title(scr, get_string(kStrInfusionModeSetting));
				}
                else if (g_set_menu_items[p_img_list->get_selected()].selected_img_id == kImgFlushMaxDoseSelected) {
					screen_set_title(scr, get_string(kStrSetMaxCleanTubeDose));
				}
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
void app_set_menu_enter(void)
{   
	screen_enter(kScreenScrSetMenu, app_set_menu_callback, NULL,
		g_set_menu_widgets, ssz_array_size(g_set_menu_widgets));
}
