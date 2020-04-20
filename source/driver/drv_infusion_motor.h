/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-14 jlli
* Initial revision.
*
************************************************/
#pragma once

/************************************************
* Declaration
************************************************/
#ifdef SSZ_TARGET_MACHINE
#define INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE	1
#else
#define INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE	0
#endif
#define INFUSION_MOTOR_START_PWM_RUN_TIME_MS 2  // 10
#define INFUSION_MOTOR_START_PWM_MAX_LEVEL 501 


#ifdef __cplusplus
extern "C" {
#endif

extern int g_infusion_motor_run_start_pwm_level ;
extern int g_infusion_motor_pwm_step_num  ;


void drv_infusion_motor_sleep_enable();
void drv_infusion_motor_sleep_disable();

void drv_infusion_motor_move_forward(int duty_cycle);
void drv_infusion_motor_move_forward_ex();
void drv_infusion_motor_move_backward(int duty_cycle);

void drv_infusion_motor_brake();
void drv_infusion_motor_inertial_stop();

void drv_infusion_motor_change_duty_cycle(int duty_cycle);
void drv_infusion_motor_init();

#ifdef __cplusplus
}
#endif


