/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-25 jlli
* Initial revision.
*
************************************************/
#include "app_scr_task_confirm.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_common.h"
#include "ui_page_control.h"
#include "ui_lable.h"
#include "ui.h"
#include "font_data.h"
#include "app_delivery.h"
#include "app_home.h"
#include "record_log.h"
#include "param.h"
#include "data.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_scr_task_confirm_widgets[] = {
    kWidgetAttrBase,kUILable, kUI_IDLable, 0,47,30,16,
    kWidgetAttrTextID,kStrReturn,kWidgetAttrAlign, UI_ALIGN_VCENTER,
    kWidgetAttrBase,kUIPageControl, kUI_IDPageControl,
    64,48,SCREEN_WIDTH - 64 * 2,11,//page control
    
	kWidgetAttrBase,kUILable, kUI_IDLable, 177,47,90,16,
    kWidgetAttrTextID,kStrConfirmInfusionSettingDetails,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_LEFT,
    kWidgetAttrBase,kUILable, kUI_IDLable1, 4,4,256,16,
    kWidgetAttrTextID,kStrDynamic1,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_LEFT,
    kWidgetAttrBase,kUILable, kUI_IDLable2, 4,24,256,16,
    kWidgetAttrTextID,kStrDynamic2,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_LEFT,

    //kWidgetAttrBase,kUILable, kUI_IDLable3, 0,34,256,10,
    //kWidgetAttrTextID,kStrDynamic3,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_LEFT,
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
static void app_scr_task_confirm_on_press_ok_key(KeyID key, int repeat_count)
{
	UserSetInfusionPara user_set_infusion_para = g_user_set_infusion_para;

	//change to infusion unit
	user_set_infusion_para.total_dose = g_user_set_infusion_para.total_dose *1000;
	user_set_infusion_para.meds_b_total_dose = g_user_set_infusion_para.meds_b_total_dose*1000;
    if(g_user_set_infusion_para.infusion_speed*100<=MIN_INFUSION_SPEED){
        g_user_set_infusion_para.infusion_speed =MIN_INFUSION_SPEED/100;
    }
	user_set_infusion_para.infusion_speed = g_user_set_infusion_para.infusion_speed *100;
	user_set_infusion_para.is_peek_infusion_mode = g_user_set_infusion_para.is_peek_infusion_mode;
	user_set_infusion_para.peek_infusion_start_time_point = g_user_set_infusion_para.peek_infusion_start_time_point;
	user_set_infusion_para.peek_infusion_end_time_point = g_user_set_infusion_para.peek_infusion_end_time_point;
	user_set_infusion_para.peek_infusion_speed = g_user_set_infusion_para.peek_infusion_speed * 100;

	if (app_delivery_state(kNormalInfusion) == kInfusionReady ||
		app_delivery_state(kNormalInfusion) == kInfusionStop) {
		//new task
		InfusionParam param;
		ssz_mem_zero(&param, sizeof(param));
		param.total_dose = user_set_infusion_para.total_dose;
		param.meds_b_total_dose = user_set_infusion_para.meds_b_total_dose;
		param.infusion_speed = user_set_infusion_para.infusion_speed;
		param.is_peek_infusion_mode = user_set_infusion_para.is_peek_infusion_mode;
		param.peek_infusion_start_time_point = user_set_infusion_para.peek_infusion_start_time_point;
		param.peek_infusion_end_time_point = user_set_infusion_para.peek_infusion_end_time_point;
		param.peek_infusion_speed = user_set_infusion_para.peek_infusion_speed;

		app_delivery_set_infusion_param(kNormalInfusion, &param, 0, 0);

		//save the para
		UserSetInfusionParaToSave info;
		info.total_dose = g_user_set_infusion_para.total_dose ;
		info.meds_b_total_dose = g_user_set_infusion_para.meds_b_total_dose ;
		info.infusion_speed = g_user_set_infusion_para.infusion_speed ;
		info.infusion_duration = g_user_set_infusion_para.infusion_duration ;
		info.peek_infusion_start_time_point = g_user_set_infusion_para.peek_infusion_start_time_point ;
		info.peek_infusion_end_time_point = g_user_set_infusion_para.peek_infusion_end_time_point ;
		info.peek_infusion_speed = g_user_set_infusion_para.peek_infusion_speed ;
		data_write(kDataSetInfusionParaInfoBlock, &info, sizeof(info));
	}
	else {
		//edit task
		InfusionInfo* info = app_delivery_info(kNormalInfusion);
		bool is_peek_set_changed = false;
		int tmp;

		tmp = info->total_dose - info->infused_dose;
		if (tmp < 0) {
			tmp = 0;
		}
		if (tmp / 1000 != user_set_infusion_para.total_dose/1000) {
			//user changed the total dose
			info->total_dose = user_set_infusion_para.total_dose;
			LogVar log_var[1];
			log_var[0].type = kLogVarInteger;
			log_var[0].value = info->total_dose / 1000;
			record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
				kLogEventChangeInfusionVolume, log_var, ssz_array_size(log_var));
		}

		tmp = info->meds_b_total_dose - info->meds_b_infused_dose;
		if (tmp < 0) {
			tmp = 0;
		}
		if (tmp / 1000 != user_set_infusion_para.meds_b_total_dose/1000) {
			//user changed the tube dose
			info->meds_b_total_dose = user_set_infusion_para.meds_b_total_dose;
			LogVar log_var[1];
			log_var[0].type = kLogVarInteger;
			log_var[0].value = info->meds_b_total_dose / 1000;
			record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
				kLogEventChangeCleanTubeVolume, log_var, ssz_array_size(log_var));
		}

		if (info->infusion_speed != user_set_infusion_para.infusion_speed) {
			info->infusion_speed = user_set_infusion_para.infusion_speed;
			LogVar log_var[1];
			log_var[0].type = kLogVarFloatWithOneDecimal;
			log_var[0].value = info->infusion_speed/ 100;
			record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
				kLogEventChangeInfusionSpeed, log_var, ssz_array_size(log_var));
		}

		if (info->is_peek_infusion_mode != user_set_infusion_para.is_peek_infusion_mode) {
			is_peek_set_changed = true;
		}
		else if (user_set_infusion_para.is_peek_infusion_mode){
			//not change the switch, check if value changed
			if (info->peek_infusion_speed != user_set_infusion_para.peek_infusion_speed ||
				info->peek_infusion_start_time_point != user_set_infusion_para.peek_infusion_start_time_point ||
				info->peek_infusion_end_time_point != user_set_infusion_para.peek_infusion_end_time_point){
				is_peek_set_changed = true;
			}
		}
		if (is_peek_set_changed) {
			info->is_peek_infusion_mode = user_set_infusion_para.is_peek_infusion_mode;
			info->peek_infusion_start_time_point = user_set_infusion_para.peek_infusion_start_time_point;
			info->peek_infusion_end_time_point = user_set_infusion_para.peek_infusion_end_time_point;
			info->peek_infusion_speed = user_set_infusion_para.peek_infusion_speed;
			if (info->is_peek_infusion_mode) {
				LogVar log_var[3];

				log_var[0].type = kLogVarInteger;
				log_var[0].value = info->peek_infusion_start_time_point;
				log_var[1].type = kLogVarInteger;
				log_var[1].value = info->peek_infusion_end_time_point;
				log_var[2].type = kLogVarFloatWithOneDecimal;
				log_var[2].value = info->peek_infusion_speed / 100;
				record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
					kLogEventChangeInfusionPeekSet, log_var, ssz_array_size(log_var));
			}
			else {
				record_log_add(kHistoryLog, LOG_EVENT_TYPE_INFUSION,
					kLogEventCloseInfusionPeekSet, NULL, 0);
			}
		}
	}
	app_delivery_save_infusion_info(kNormalInfusion);
	screen_go_back_to_id(kScreenHome);
}

/***********************************************
* Description:
*   handle back key
* Argument:
*   repeat_count: 0:first press, 1: repeat once after hold, 2: repeat twice after hold...  
* Return:
*
************************************************/
static void app_scr_task_confirm_on_press_back_key(KeyID key, int repeat_count)
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
static void app_scr_task_confirm_on_scr_create(Screen* scr)
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
static void app_scr_task_confirm_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	UIPageControl* p_page_control = (UIPageControl*)scr_view->get_child(kUI_IDPageControl);
	int total_time;

	p_page_control->set_size(2);
	if (get_user_infusion_para(kIsPeekInfusionMode)) {
		p_page_control->show();
	} else {
		p_page_control->hide();
		key_set_handler(kKeyLeft, kKeyEventPressed, NULL);
		key_set_handler(kKeyRight, kKeyEventPressed, NULL);
	}

	char *p = get_dynamic_string(kStrDynamic1);
	sprintf(p, "%s: %dml, %s: %dml", get_string(kStrTotalDose), get_user_infusion_para(kTotalDose), get_string(kStrCleanTubeDose),
		get_user_infusion_para(kCleanTubeDose));
	p = get_dynamic_string(kStrDynamic2);

	InfusionInfo tmp_info;

	ssz_mem_zero(&tmp_info, sizeof(tmp_info));
	tmp_info.total_dose = g_user_set_infusion_para.total_dose *1000;
    tmp_info.infusion_speed = g_user_set_infusion_para.infusion_speed *100;
	tmp_info.is_peek_infusion_mode = g_user_set_infusion_para.is_peek_infusion_mode;
	tmp_info.peek_infusion_start_time_point = g_user_set_infusion_para.peek_infusion_start_time_point;
	tmp_info.peek_infusion_end_time_point = g_user_set_infusion_para.peek_infusion_end_time_point;
	tmp_info.peek_infusion_speed = g_user_set_infusion_para.peek_infusion_speed * 100;

	total_time = app_delivery_calc_a_bag_rest_time_secs(&tmp_info)/60 + 60 * g_user_set_infusion_para.meds_b_total_dose / FLUSH_SPEED; //in minute

	if (get_user_infusion_para(kInfusionSpeed) > 0) {
	sprintf(p, "%s: %dhr, %dmin, %s: %d.%dml/hr", get_string(kStrInfusionDuration),
		total_time / 60, total_time % 60, get_string(kStrSpeed), get_user_infusion_para(kInfusionSpeed) / 10,
		get_user_infusion_para(kInfusionSpeed) % 10);
	} else {
		sprintf(p, "%s: -hr, %s: %d.%dml/hr", get_string(kStrInfusionDuration), get_string(kStrSpeed), get_user_infusion_para(kInfusionSpeed) / 10,
			get_user_infusion_para(kInfusionSpeed) % 10);
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
static void app_scr_task_confirm_on_scr_activate(Screen* scr, UIView* scr_view)
{
    //bind key
    key_set_handler(kKeyOK, kKeyEventPressed, app_scr_task_confirm_on_press_ok_key);
    key_set_handler(kKeyBack, kKeyEventPressed, app_scr_task_confirm_on_press_back_key);
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
static void app_scr_task_confirm_callback(Screen* scr, ScreenMsg * msg)
{
	switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            //init all widgets
            app_scr_task_confirm_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_scr_task_confirm_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_scr_task_confirm_on_scr_activate(scr, scr->view);
            break;
        case kUIMsgScrDeactivate:
            break;
        case kUIMsgScrDestroy:
            break;
        case kUIMsgNotifyParent:
            if (msg->int_param==kUINotifySelectChanged &&
                msg->src->id()==kUI_IDPageControl){
				UIPageControl* p = (UIPageControl*)scr->view->get_child(kUI_IDPageControl);
				UILable* lable = (UILable*)scr->view->get_child(kUI_IDLable1);
                if(p->get_selected()==0){
                    lable= (UILable*)scr->view->get_child(kUI_IDLable1);
                    lable->show();
                    lable= (UILable*)scr->view->get_child(kUI_IDLable2);
                    lable->show();
                }else{
                    lable= (UILable*)scr->view->get_child(kUI_IDLable1);
                    lable->hide();
                    lable= (UILable*)scr->view->get_child(kUI_IDLable2);
                    lable->hide();
                }
				scr->view->invalidate();
            }
            break;
        case kUIMsgForceDrawAfterAllDraw:
		{

		}
        break;
        case kUIMsgDraw:
		{
			ui_set_bk_color(UI_BLACK);
			ui_clear();
			UIPageControl* p = (UIPageControl*)scr->view->get_child(kUI_IDPageControl);
			ui_set_pen_color(UI_WHITE);
			ui_draw_hline_at(0, 46, 256);

            int start=get_user_infusion_para(kPeekInfusionStartTimePoint);
            int end=get_user_infusion_para(kPeekInfusionEndTimePoint);
            char ch[20];

            if (p->get_selected() == 1)
			{
				ui_set_fill_color(UI_WHITE);
				ui_fill_rect_at(0, 26, 256, 6);
                
                ui_set_text_color(UI_WHITE);
                ui_set_font(get_font(14));
              
				if(start<=end){
                    //draw numbers
					for(int i=0; i<12; i++){
						sprintf(ch, "%d",i*2);
						if(i <= 4)
							ui_disp_text_at(i*20, 32, ch);
						else
							ui_disp_text_at(80+(i-4)*21, 32, ch);
					}
					ui_disp_text_at(247,32,"H");

                    ui_fill_rect_at(start*11, 20, (end-start)*11, 6);
                    sprintf(ch, "%d.%dml/hr", get_user_infusion_para(kPeekInfusionSpeed)/10,
                        get_user_infusion_para(kPeekInfusionSpeed) %10);
					//display speed at center
					if (start>=17) {
						if (get_user_infusion_para(kPeekInfusionSpeed) < 100)
							ui_disp_text_at(256 - 50, 4, ch);
						else if (get_user_infusion_para(kPeekInfusionSpeed) < 1000)
							ui_disp_text_at(256 - 55, 4, ch);
						else
							ui_disp_text_at(256 - 65, 4, ch);
					}else{
                        if(start == 0 && end <=5)
							ui_disp_text_at(start*11,4,ch);
						else
							ui_disp_text_at(start * 11 + ((end - start) * 11 - 40) / 2, 4, ch);
					}
                }else{
					int tmp = ((24+end-start)/2%2 == 0 ? (24+end-start)/2:(24+end-start)/2+1);
                    int mid= (start+tmp>=24 ? start+tmp-24 :start+tmp)%2 == 0 ? 
						(start+tmp >= 24 ? start+tmp-24 : start+tmp): (start+tmp >= 24 ? start+tmp-24:start+tmp)+1;
                    
					for (int i=0; i<12; i ++) {				        
						if(i<6){
							if (mid+i*2>=24){
								sprintf(ch, "%d", mid+i*2-24);							
							}else{
								sprintf(ch, "%d",mid+i*2);
							}
                            ui_disp_text_at(128+i*21, 32, ch);
						}else{
							if (mid-(i-6)*2>0){
								sprintf(ch, "%d", mid-(i-6)*2-2);
								
							}else{
								sprintf(ch, "%d",24+(mid-(i-6)*2)-2);
							}
						    ui_disp_text_at(107-(i-6)*21, 32, ch);
						}
					}
					ui_disp_text_at(247, 32, "H");

                    if(mid<start)
                        ui_fill_rect_at(128-(mid+24-start)*11+11,20,(24+end-start)*11,6);
                    else
                        ui_fill_rect_at(128-(mid-start)*11+11,20,(24+end-start)*11,6);

                    sprintf(ch, "%d.%dml/hr", get_user_infusion_para(kPeekInfusionSpeed)/10,
                        get_user_infusion_para(kPeekInfusionSpeed) %10);
                    if ((24-start+end)*11>46) {
						ui_disp_text_at(128-23, 4, ch);
					}else{
						ui_disp_text_at(120-tmp*11, 4, ch);
					}
                }
			}
			return;
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
void app_scr_task_confirm_enter(void)
{   
    screen_enter_full_screen(kScreenScrTaskConfirm, app_scr_task_confirm_callback,NULL,
        g_scr_task_confirm_widgets, ssz_array_size(g_scr_task_confirm_widgets));
}




