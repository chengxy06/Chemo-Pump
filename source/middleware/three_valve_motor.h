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
#include "infusion_motor.h"

/************************************************
* Declaration
************************************************/
#define MOTOR_STOP_BY_REACH_TARGET 6

  
#define THREE_MOTOR_ALLOW_DIRECTION_ERROR_COUNT 100  // infusion motor's 20 too small, three motor need big

#define THREE_VALVE_MOTOR_START_PWM_RUN_TIME_MS 2
#define THREE_VALVE_MOTOR_START_PWM_LEVEL 100 
#define THREE_VALVE_MOTOR_START_PWM_MAX_LEVEL 500 

#define THREE_VALVE_MOTOR_CHECK_BATTERY_MS 100

#ifdef __cplusplus
extern "C" {
#endif
extern int g_three_valve_motor_encoder;
extern int g_three_valve_motor_expect_encoder;
extern bool g_three_valve_motor_is_event_set;
extern MotorState g_three_valve_motor_state;
extern MotorDirection g_three_valve_motor_running_dir;
extern int g_three_valve_motor_direction_error_count;
extern int  g_three_valve_motor_direction_error_total;


bool three_valve_motor_move_to_position_a();
bool three_valve_motor_move_to_position_b();

//start three-valve motor for the expected encoder in the set direction
void three_valve_motor_start(MotorDirection dir, int expect_encoder ,MotorStopHandler stop_handler);
void three_valve_motor_stop(int stop_cause);

MotorDirection three_valve_motor_direction();
bool three_valve_motor_is_running();
int three_valve_motor_encoder_after_start();
int three_valve_motor_expect_encoder();
int three_valve_motor_stop_cause();

void three_valve_motor_init();

#ifdef __cplusplus
}
#endif


