#pragma once

#ifdef __cplusplus
extern "C" {
#endif


//#include "stm32l1xx_hal.h"
#include "main.h"

#ifdef SSZ_TARGET_MACHINE
#define THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE	1
#else
#define THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE	0
#endif

//enable/disable all motor power supply form system power supply
void drv_all_motor_pwr_enable();
void drv_all_motor_pwr_disable();

//enable/disable all 5v power supply
void drv_all_5v_pwr_enable();
void drv_all_5v_pwr_disable();

void drv_three_valve_motor_pwr_enable();
void drv_three_valve_motor_pwr_disable();

void drv_three_valve_motor_sleep_enable();
void drv_three_valve_motor_sleep_disable();

void drv_three_valve_motor_move_forward(int duty_cycle);
void drv_three_valve_motor_move_backward(int duty_cycle);
//set three-valve motor move forward on gpio mode
void drv_three_valve_motor_move_forward_ex();
//set three-valve motor move backward on gpio mode
void drv_three_valve_motor_move_backward_ex();

void drv_three_valve_motor_brake();
void drv_three_valve_motor_inertial_stop();

void drv_three_valve_pos_detect_pwr_enable();
void drv_three_valve_pos_detect_pwr_disable();

// check if three-vavle has reached position A
bool drv_three_valve_is_pos_a_detected();
// check if three-vavle has reached position B
bool drv_three_valve_is_pos_b_detected();

void drv_three_valve_motor_change_duty_cycle_to_a(int duty_cycle);
void drv_three_valve_motor_change_duty_cycle_to_b(int duty_cycle);

void drv_three_valve_motor_init();

extern int g_three_valve_motor_run_start_pwm_level ;
extern int g_three_valve_motor_pwm_step_num ;

#ifdef __cplusplus
}
#endif




