/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-26 xqzhao
* Initial revision.
*
************************************************/
#include "app_home.h"
#include "screen.h"
#include "key.h"
#include "ui_define.h"
#include "ssz_common.h"
#include "ui_common.h"
#include "string_data.h"
#include "app_main_menu.h"
#include "ui_text_base.h"
#include "ui_image.h"
#include "ui_lable.h"
#include "app_delivery.h"
#include "timer.h"
#include "scr_tip_popup.h"
#include "app_common.h"
#include "param.h"


/************************************************
* Declaration
************************************************/
#define ID_LABLE_EXPECT_TIME kUI_IDLable2
#define ID_LABLE_DOSE_LEFT_AND_TOTAL_DOSE kUI_IDLable3
#define ID_LABLE_INFUSION_SPEED kUI_IDLable4
#define ID_LABLE_HR_UNIT kUI_IDLable5

#define AUTO_LOCK_TIMER kTimerScreen1
#define AUTO_LOCK_MS_AFTER_INFUSION 3000


/************************************************
* Variable 
************************************************/
//widgets at this screen
static const int16_t g_home_widgets[] = {
	kWidgetAttrBase,kUIStatusBar, kUI_IDStatusBar, 64,0,-1,14,//status bar

    kWidgetAttrBase,kUIImage, kUI_IDImage,153,15,20,16, //status icon
    kWidgetAttrImgID,kImgInfusionStop,

    kWidgetAttrBase,kUILable, kUI_IDLable1,169,15,87,16, //infusion status
	kWidgetAttrTextID,kStrInfusionStop,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_RIGHT,

    kWidgetAttrBase,kUILable, ID_LABLE_EXPECT_TIME, 0,0,132,64,  //expect time
    kWidgetAttrTextID,kStrDynamic1,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_HCENTER,
    kWidgetAttrFontHeight,VERY_LARGE_FONT_HEIGHT,

    kWidgetAttrBase,kUILable, kUI_IDLable, 153,31,118,16,
    kWidgetAttrTextID,kStrInfusedDose,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_LEFT,

    kWidgetAttrBase,kUILable, ID_LABLE_DOSE_LEFT_AND_TOTAL_DOSE,-62,31,62,16, //dose left/total dose
    kWidgetAttrTextID,kStrDynamic2,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_RIGHT,

    kWidgetAttrBase,kUILable, kUI_IDLable, 153,47,88,16,
    kWidgetAttrTextID,kStrSpeed,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_LEFT,

    kWidgetAttrBase,kUILable, ID_LABLE_INFUSION_SPEED,-78,47,78,15, //infusion speed
    kWidgetAttrTextID,kStrDynamic3,kWidgetAttrAlign, UI_ALIGN_VCENTER|UI_ALIGN_RIGHT,

    kWidgetAttrBase,kUILable, ID_LABLE_HR_UNIT,132,49,20,14, //hour
    kWidgetAttrTextID,kStrHour,kWidgetAttrAlign, UI_ALIGN_VCENTER| UI_ALIGN_HCENTER,
};

static InfusionState g_last_infusion_state;
static InfusionBagMeds g_last_infusion_bag;
static int g_last_expect_infusion_time;
static int g_last_infused_dose;
static int g_last_total_dose;
static int g_last_infusion_speed;
static int g_last_expect_infusion_hour_char_len;

/************************************************
* Function 
************************************************/
static void app_home_update_screen()
{
	UIImage* p ;
    UILable* lable;
    char *str;
    int total_expect_infusion_time_secs=0;
    int expect_infusion_time=0;
	int expect_infusion_hour_char_len;
	int cur_infused_dose = 0;
	int cur_total_dose = 0;
	int cur_infusion_speed = 0;
	InfusionBagMeds cur_infuse_bag = app_delivery_curr_bag(kNormalInfusion);
    bool is_delivery_in_peak_mode = false;

    //set icon and remind
    p = (UIImage*)screen_current()->view->get_child(kUI_IDImage);
    lable = (UILable*)screen_current()->view->get_child(kUI_IDLable1);
    is_delivery_in_peak_mode = app_delivery_is_in_peak_mode();

    if(app_delivery_state(kNormalInfusion) != g_last_infusion_state){
        if(app_delivery_state(kNormalInfusion) == kInfusionRunning){
            p->set_img(get_image(kImgInfusionRun));
 			if (cur_infuse_bag == kBagAMeds) {
                if (is_delivery_in_peak_mode) {
                    lable->set_text(get_string(kStrPeakModeInfusing));
                } else {
                    lable->set_text(get_string(kStrInfusionMedsBag1));
                }
 			}else if (cur_infuse_bag == kBagBMeds) {
 				lable->set_text(get_string(kStrInfusionMedsBag2));
			}else {
				lable->set_text(get_string(kStrInfusing));
			}         
			g_last_infusion_bag = cur_infuse_bag;
        }else if(app_delivery_state(kNormalInfusion) == kInfusionPause){
            p->set_img(get_image(kImgInfusionPause));            
			lable->set_text(get_string(kStrInfusionPause));			
        }else{
            p->set_img(get_image(kImgInfusionStop));
            lable->set_text(get_string(kStrInfusionStop));
        }
    }
    g_last_infusion_state=app_delivery_state(kNormalInfusion);

	if (app_delivery_state(kNormalInfusion) == kInfusionRunning&&
		g_last_infusion_bag != cur_infuse_bag) {
		g_last_infusion_bag = cur_infuse_bag;
		if (g_last_infusion_bag == kBagAMeds) {
			lable->set_text(get_string(kStrInfusionMedsBag1));
		}
		else if (g_last_infusion_bag == kBagBMeds) {
			lable->set_text(get_string(kStrInfusionMedsBag2));
		}
	}

    //update expect time
    str = get_dynamic_string(kStrDynamic1);
    total_expect_infusion_time_secs=app_delivery_a_bag_rest_time_secs(kNormalInfusion)+app_delivery_b_bag_rest_time_secs(kNormalInfusion);
    lable = (UILable*)screen_current()->view->get_child(ID_LABLE_HR_UNIT);
    if(total_expect_infusion_time_secs >= 3600){
        expect_infusion_time= total_expect_infusion_time_secs/3600;
        lable->set_text(get_string(kStrHour));
    }else{
        expect_infusion_time= total_expect_infusion_time_secs/60;
        lable->set_text(get_string(kStrMinute));
    }
    if(expect_infusion_time < 1000)
        sprintf(str, "%d", expect_infusion_time);
    else
        sprintf(str, "%d", 999);

    expect_infusion_hour_char_len = strlen(str);
	lable = (UILable*)screen_current()->view->get_child(ID_LABLE_EXPECT_TIME);
    if(g_last_expect_infusion_time != expect_infusion_time)
	    lable->invalidate();
    g_last_expect_infusion_time=expect_infusion_time;

	//update infused dose/total dose
	if (cur_infuse_bag == kBagAMeds){
		cur_infused_dose = app_delivery_infused_dose(kNormalInfusion);
		cur_total_dose = app_delivery_info(kNormalInfusion)->total_dose;
	}
	else if(cur_infuse_bag == kBagBMeds){
		cur_infused_dose = app_delivery_meds_b_infused_dose(kNormalInfusion);
		cur_total_dose = app_delivery_info(kNormalInfusion)->meds_b_total_dose;
	}
    str = get_dynamic_string(kStrDynamic2);
    sprintf(str, "%d/%dml", cur_infused_dose/1000, cur_total_dose/1000 );
   lable = (UILable*)screen_current()->view->get_child(ID_LABLE_DOSE_LEFT_AND_TOTAL_DOSE);
    if(g_last_infused_dose != cur_infused_dose || cur_total_dose != g_last_total_dose)
        lable->invalidate();
    g_last_infused_dose=  cur_infused_dose;
    g_last_total_dose= cur_total_dose;

	//update infusion speed
	if (cur_infuse_bag == kBagAMeds){
        if (is_delivery_in_peak_mode) {
            cur_infusion_speed = app_delivery_info(kNormalInfusion)->peek_infusion_speed;
        } else {
            cur_infusion_speed = app_delivery_info(kNormalInfusion)->infusion_speed;
        }
	} else if (cur_infuse_bag == kBagBMeds) {
		cur_infusion_speed = BAG_B_MEDS_INFUSION_SPEED;
	}

    str = get_dynamic_string(kStrDynamic3);
	if (cur_infusion_speed == BAG_B_MEDS_INFUSION_SPEED) {
		sprintf(str, "%s", "MAX");
	}
	else {
		sprintf(str, "%d.%dml/hr", cur_infusion_speed / 1000, cur_infusion_speed / 100 % 10);
	}
    lable = (UILable*)screen_current()->view->get_child(ID_LABLE_INFUSION_SPEED);
    if(g_last_infusion_speed != cur_infusion_speed)
        lable->invalidate();
	g_last_infusion_speed = cur_infusion_speed;

    //set unit "hr" position
    lable = (UILable*)screen_current()->view->get_child(ID_LABLE_HR_UNIT);
    if(g_last_expect_infusion_hour_char_len != expect_infusion_hour_char_len){
        if(expect_infusion_hour_char_len==1){
            lable->set_pos(88,49);   //1 digit
        }else if(expect_infusion_hour_char_len==2){
            lable->set_pos(112,49);  //2 digits
        }else{
            lable->set_pos(132,49);  //3 digits
        }
    }
    g_last_expect_infusion_hour_char_len=expect_infusion_hour_char_len;
}

/***********************************************
* Description:
*   handle ok key
* Argument:
*
* Return:
*
************************************************/
static void app_home_on_press_ok_key(KeyID key, int repeat_count)
{
	if (app_delivery_state(kNormalInfusion) != kInfusionRunning) {
		app_main_menu_enter();
	}
}

static void app_home_on_press_start_key(KeyID key, int repeat_count)
{
	if (app_delivery_state(kNormalInfusion) == kInfusionReady) {
		app_delivery_start(kNormalInfusion);
		if (!is_keypad_locked()) {
			timer_start_oneshot_after(AUTO_LOCK_TIMER, AUTO_LOCK_MS_AFTER_INFUSION);
		}
	}
	else if (app_delivery_state(kNormalInfusion) == kInfusionRunning) {
		app_delivery_pause(kNormalInfusion);
		timer_stop(AUTO_LOCK_TIMER);
	}
	else {
		app_delivery_resume(kNormalInfusion);
		if (!is_keypad_locked()) {
			timer_start_oneshot_after(AUTO_LOCK_TIMER, AUTO_LOCK_MS_AFTER_INFUSION);
		}
	}
	app_home_update_screen();
}

/***********************************************
* Description:
*   handle screen create msg
* Argument:
*
* Return:
*
************************************************/
static void app_home_on_scr_create(Screen* scr)
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
static void app_home_on_scr_widget_init(Screen* scr, UIView* scr_view)
{
	//add item
	UILable* p = (UILable*)scr_view->get_child(kUI_IDLable1);
	p->set_bk_color(UI_WHITE);
    p ->set_text_color(UI_BLACK);
}

/***********************************************
* Description:
*   handle screen active msg
* Argument:
*
* Return:
*
************************************************/
static void app_home_on_scr_activate(Screen* scr)
{
    //bind key
	key_set_handler(kKeyOK, kKeyEventPressed, app_home_on_press_ok_key);
	key_set_handler(kKeyStart, kKeyEventPressed, app_home_on_press_start_key);

    g_last_infusion_state=InfusionState(-1);
	g_last_infusion_bag = kBagAMeds;
    g_last_expect_infusion_time=-1;
    g_last_infused_dose=-1;
    g_last_total_dose=-1;
    g_last_infusion_speed=-1;
    g_last_expect_infusion_hour_char_len=-1;

    //update screen
	screen_set_timer_handler(kTimerScreen, app_home_update_screen);
    timer_start_periodic_every(kTimerScreen, TIMER_UPDATE_SCREEN);
	app_home_update_screen();

	screen_set_timer_handler(AUTO_LOCK_TIMER, lock_keypad);
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
static void app_home_callback(Screen* scr, ScreenMsg * msg)
{
    switch(msg->msg_id)
    {
        case kUIMsgScrCreate:
            app_home_on_scr_create(scr);
            break;
		case kUIMsgScrWidgetInit:
			app_home_on_scr_widget_init(scr, scr->view);
			break;
        case kUIMsgScrActive:
            app_home_on_scr_activate(scr);
            break;
        case kUIMsgScrDeactivate:
			statusbar()->hide_icon(kStatusbarIconOrderLock);
            break;
        case kUIMsgScrDestroy:
            break;
		case kUIMsgDeliveryStateChanged:
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
void app_home_enter(void)
{   
     screen_enter_full_screen(kScreenHome, app_home_callback,NULL,  
        g_home_widgets, ssz_array_size(g_home_widgets));
}

