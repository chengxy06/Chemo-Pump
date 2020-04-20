/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-10 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "string_data.h"
#include "image_data.h"
#include "app_delivery.h"

/************************************************
* Declaration
************************************************/
#define TEST_LIFETIME_ENABLE 1

typedef struct{
	StrID str_id;
	ImgID img_id;
}StrImgIDItem;

typedef struct {
	StrID str_id;
	ImgID img_id;
	ImgID selected_img_id;
}StrWithTwoImgIDItem;

typedef struct{
	int total_dose;           //total dose to be infused, unit: 1ml
	//int clean_tube_dose;      //clean tube dose, 1ml
	int meds_b_total_dose;     //(clean tube)dose set to be infused, unit:ml	

    //limit_infusion_speed, 0.1ml/h, has enlarged 10 times, 
    //i.e: if the value is 1ml/hr, real speed is 0.1ml/hr
    //all speed related parameters has the same mode
    int limit_infusion_speed; 
    int infusion_speed;       //unit: 0.1ml/hr
    int limit_infusion_duration; //unit: 0.1hr
    int infusion_duration;       //unit: minute
    bool is_peek_infusion_mode;
    int peek_infusion_start_time_point;  //start time point, 0-23h
    int peek_infusion_end_time_point;    //end time point, 0-23h
    int peek_infusion_speed;//unit: 0.1ml/hr
}UserSetInfusionPara;

typedef struct{
	int total_dose;           //total dose to be infused, unit: 1ml
	//int clean_tube_dose;      //clean tube dose, 1ml
	int meds_b_total_dose;     //(clean tube)dose set to be infused, unit:ml	

    int infusion_speed;       //unit: 0.1ml/hr
    int infusion_duration;       //unit: 0.1hr
    int peek_infusion_start_time_point;  //start time point, 0-23h
    int peek_infusion_end_time_point;    //end time point, 0-23h
    int peek_infusion_speed;//unit: 0.1ml/hr
}UserSetInfusionParaToSave;


typedef enum{
	kTotalDose,
	kCleanTubeDose,
    kLimitInfusionSpeed, //ml/h
    kInfusionSpeed,
    kLimitInfusionDuration, //h
    kInfusionDuration,
    kIsPeekInfusionMode,
    kPeekInfusionStartTimePoint,
    kPeekInfusionEndTimePoint,
    kPeekInfusionSpeed,
    kUserInfusionParaMax
}UserSetInfusionParaType;

/* for the line equation: y=ax+b*/
typedef struct{
    int a;    /* slope, zoom 1000*/
    int b;    /* intercept, zoom 1000*/
}LineEquationPara;

#if TEST_LIFETIME_ENABLE
extern bool g_is_testing_lifetime;
extern InfusionParam g_test_lifetime_infusion_param;
#endif

extern UserSetInfusionPara g_user_set_infusion_para;
//true: new task, false:edit task
//extern bool g_is_user_set_new_task;
void set_all_user_infusion_para(UserSetInfusionParaType *infusion_data);
void set_user_infusion_para(UserSetInfusionParaType type, int set_data);
int get_user_infusion_para(UserSetInfusionParaType type);

void set_sleep_secs_before_wakeup(int sec);
int get_sleep_secs_before_wakeup(void);

void power_off();
bool is_auto_power_off();
void set_auto_power_off(bool is_auto_power_off);

void enter_factory_mode();
void exit_factory_mode();

bool is_allow_check_no_operate(void);

void lock_keypad(); 
void unlock_keypad();
bool is_keypad_locked();

void set_sleep_allow(bool is_allow_sleep);
bool is_sleep_allow();

void prepare_pressure_adc_before_infusion_start();

//clear watchdog
void clear_partner_mcu_watchdog();
void delay_ms_without_watchdog(int delay_ms);

#ifdef __cplusplus
extern "C" {
#endif
void watchdog_start();
void watchdog_stop();
bool watchdog_is_enable();
void watchdog_clear();
void watchdog_set_expired_time(int expired_time);

#ifdef __cplusplus
}
#endif

//calc the a and b of the line, the line equation is: y=ax+b
LineEquationPara fit_line_by_data(int x[], int y[], int count);
