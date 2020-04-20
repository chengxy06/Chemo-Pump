/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-10 jlli
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
//#include "app_common.h"

/************************************************
* Declaration
************************************************/
//param for start msg
#define DELIVERY_START_TO_RUN 0 
#define DELIVERY_START_TO_PAUSE 1 
 
//param for pause msg
#define DELIVERY_PAUSE_BY_OTHER 0
#define DELIVERY_PAUSE_BY_FINISH_DELIVERY 1

//param for resume msg


//param for stop msg
#define DELIVERY_STOP_FROM_RUN 0
#define DELIVERY_STOP_FROM_PAUSE 1
typedef enum
{
	kBagBull,
    kBagAMeds,
    kBagBMeds,
}InfusionBagMeds;
typedef enum
{
	kNormalInfusion,
	kCleanTube,
	kClearAir,
	kInfusionTypeMax,
}InfusionType;
typedef enum
{
	kInfusionStop,
	kInfusionReady,
    kInfusionRunning,
	kInfusionPause,
}InfusionState;

typedef struct
{
    int total_dose;     //A bag dose set to be infused, unit:0.001ml
	int meds_b_total_dose;     //B bag dose set to be infused, unit:0.001ml	
	int meds_b_additional_dose;     //B bag additional dose to be infused, unit:0.001ml	
    //int first_infusion_dose;//A bag first infusion dose, unit:0.001ml
	int infusion_speed; //A bag infused speed, unit:0.001ml/hr
	bool is_peek_infusion_mode;
	int peek_infusion_start_time_point;  //start time point, 0-23hr
	int peek_infusion_end_time_point;    //end time point, 0-23hr
	int peek_infusion_speed; //A bag peed infused speed, unit:0.001ml/hr

    int infused_dose;//A bag infused dose, unit:0.001ml
	int meds_b_infused_dose; //B bag infused dose, unit:0.001ml

    //InfusionBagMeds curr_meds_type;
	//int32_t infuse_start_time;
    InfusionState infusion_state;
	//InfusionType infusion_type;
}InfusionInfo;

typedef struct {
	int total_dose;     //A bag dose set to be infused, unit:0.001ml
	int meds_b_total_dose;     //B bag dose set to be infused, unit:0.001ml	
	//int first_infusion_dose;//A bag first infusion dose, unit:0.001ml
	int infusion_speed; //A bag infused speed, unit:0.001ml/hr

	bool is_peek_infusion_mode;
	int peek_infusion_start_time_point;  //start time point, 0-23hr
	int peek_infusion_end_time_point;    //end time point, 0-23hr
	int peek_infusion_speed; //A bag peed infused speed, unit:0.001ml/hr
}InfusionParam;

typedef struct {
	bool is_use_rtc;//true:use rtc time, false:use systick time

	//RTC time
	int32_t rtc_seconds;//seconds since base time 
	int32_t rtc_millisecond;//ms after seconds

	//systick time
	int32_t tick_time;
}InfusionTime;

#ifdef __cplusplus
    extern "C" {
#endif

void app_delivery_init();
//set the state as ready
void app_delivery_set_ready(InfusionType infusion_type);
//all variable's unit is 0.001ml
//param.total_dose: -1->will never auto stop when delivery A bag
//param.meds_b_total_dose: -1->will never auto stop when delivery B bag
// it will set the state as ready
void app_delivery_set_infusion_param(InfusionType infusion_type, InfusionParam* param, int infused_dose, int meds_b_infused_dose);
//it will send start msg (msg param1:infusion_type, param2: DELIVERY_START_TO_RUN)
bool app_delivery_start(InfusionType infusion_type);
//start to pause
//it will send start msg (msg param1:infusion_type, param2: DELIVERY_START_TO_PAUSE)
bool app_delivery_start_to_pause(InfusionType infusion_type);

//all variable's unit is 0.001ml
//param.total_dose: -1->will never auto stop when delivery A bag
//param.meds_b_total_dose: -1->will never auto stop when delivery B bag
//it will send start msg (msg param1:infusion_type, param2: DELIVERY_START_TO_RUN)
bool app_delivery_start_with_param(InfusionType infusion_type, InfusionParam* param);
//it will send pause msg (msg param1:infusion_type, param2: DELIVERY_PAUSE_BY_OTHER)
void app_delivery_pause(InfusionType infusion_type);
//it will send resume msg (msg param1:infusion_type, param2: )
bool app_delivery_resume(InfusionType infusion_type);
//it will send stop msg (msg param1:infusion_type, param2: from_state, e.g. DELIVERY_STOP_FROM_RUN)
void app_delivery_stop(InfusionType infusion_type);

InfusionState app_delivery_state(InfusionType infusion_type);
InfusionBagMeds app_delivery_curr_bag(InfusionType infusion_type);
int app_delivery_curr_speed(InfusionType infusion_type);
bool app_delivery_is_infusing_one_point(InfusionType infusion_type);

//unit:0.001ml
int app_delivery_infused_dose(InfusionType infusion_type);
//unit:0.001ml
int app_delivery_meds_b_infused_dose(InfusionType infusion_type);

int app_delivery_calc_a_bag_rest_time_secs(InfusionInfo *infusion_info);

//return the rest seconds to complete delivery a bag
//-1: never can complete
int app_delivery_a_bag_rest_time_secs(InfusionType infusion_type);
int app_delivery_b_bag_rest_time_secs(InfusionType infusion_type);

//is the infusion arrive the target
bool app_delivery_is_finish_expect_target(InfusionType infusion_type);
bool app_delivery_is_finish_expect_target_ex(InfusionInfo *info);

InfusionInfo* app_delivery_info(InfusionType infusion_type);
void app_delivery_save_infusion_info(InfusionType infusion_type);

//check if exist some infusion type running
bool app_delivery_is_exit_running_infusion();
//return current running infusion type, if no run, return kInfusionTypeMax
InfusionType app_delivery_curr_running_infusion();

//return next delivery point time, 
// if use_rtc=1 and rtc_second=-1, it indicate no next infuse point
// Note: it need call when it not infusing point
InfusionTime app_delivery_next_infuse_point_time();
//return the diff ms from now to next delivery point
// when use RTC, because RTC not have millisecond,
//	so the value will have deviation which < 1 second
//if return INT32_MAX, it indicate no next infuse point
// Note: it need call when it not infusing point
int32_t  app_delivery_next_infuse_point_diff_ms_from_now();

//not wait next delivery time, infuse the next point at once
void app_delivery_infuse_next_one_point_at_once();

//delivery is in peak mode
bool app_delivery_is_in_peak_mode(void);

#ifdef __cplusplus
    }
#endif


