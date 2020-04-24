/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-21 jlli
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "common.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


extern int g_infusion_monitor_battery_empty_voltage_for_msg;

#define motor_printfln(...) ssz_printfln_ex(is_module_enable_output(kModuleMotor),MOTOR_MODULE_NAME,__VA_ARGS__)
#define motor_monitor_printfln(...) ssz_printfln_ex(is_module_enable_output(kModuleMotorMonitor),MOTOR_MONITOR_MODULE_NAME,__VA_ARGS__)
#define MOTOR_STOP_BY_USER 0	
#define MOTOR_STOP_BY_FINISH_EXPECT_TARGET 1	
#define MOTOR_STOP_BY_STUCK 2
#define MOTOR_STOP_BY_DIRECTION_WRONG 3
#define MOTOR_STOP_BY_CURRENT_TOO_LARGE 4
#define MOTOR_STOP_BY_OPTICAL_COUPLER_ERROR 5
#define MOTOR_STOP_BY_BATTERY_LOW 6

#define MOTOR_STOP_CUSTOM_CAUSE_START  10  //other cause should start after this value	
//the allow error count at once run
#define MOTOR_ALLOW_DIRECTION_ERROR_COUNT 20
#define MOTOR_ALLOW_MOTOR_CURRENT_ERROR_COUNT 2
  
  
#define MOTOR_ENABLE_CHECK_ENCODER_AFTER_STOP 0
#define MOTOR_ENABLE_CALC_SPEED 0
#if MOTOR_ENABLE_CALC_SPEED
#define MOTOR_SPEED_CHECK_COUNT 5000
#endif

typedef enum
{
    kForward,
    kBackward
}MotorDirection;

typedef enum
{
    kMotorStop,
    kMotorRun,
    kMotorStateMax
}MotorState;

typedef void(*MotorStopHandler)(int run_encoder, int stop_cause);

extern int g_infusion_motor_encoder;
extern int g_infusion_motor_optical_coupler_encoder; // 光耦计数
#if MOTOR_ENABLE_CHECK_ENCODER_AFTER_STOP
extern int g_infusion_motor_forward_encoder_after_stop;
extern int g_infusion_motor_backward_encoder_after_stop;
#endif
extern int g_infusion_motor_expect_encoder;
extern int g_infusion_motor_expect_optical_coupler_encoder; // 
extern bool g_infusion_motor_is_event_set;

extern MotorDirection g_infusion_motor_running_dir;
extern MotorState g_infusion_motor_state;
extern int g_infusion_motor_direction_error_count;
extern int  g_infusion_motor_a_b_direction_error_total;


#if MOTOR_ENABLE_CALC_SPEED
extern int g_infusion_motor_check_speed_encoder;
extern bool g_infusion_motor_is_speed_done;
extern int g_infusion_motor_check_speed_encoder_start_time;
extern int g_infusion_motor_check_speed_encoder_run_time;
#endif
//启动时判断是否可以运行
extern int g_infusion_motor_is_cannot_run;


//start infusion motor for the expected encoder in the set direction
bool infusion_motor_start(MotorDirection dir, int expect_encoder, MotorStopHandler stop_handler);
void infusion_motor_stop(int stop_cause);
//stop by speed encoder
bool infusion_motor_start_ex(MotorDirection dir, int expect_encoder, MotorStopHandler stop_handler);
bool infusion_motor_start_internal(MotorDirection dir, int expect_encoder,int expect_optical_coupler_encoder, MotorStopHandler 
stop_handler);

//return the encoder from start run
int infusion_motor_encoder_after_start();
int infusion_motor_coupler_encoder_after_start();
int infusion_motor_expect_encoder();
MotorDirection infusion_motor_direction();
bool infusion_motor_is_running();
int infusion_motor_stop_cause();
void infusion_motor_init();



#ifdef __cplusplus
}
#endif


