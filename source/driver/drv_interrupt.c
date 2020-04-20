/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-07 xqzhao
* Initial revision.
*
************************************************/
#include "drv_interrupt.h"
#include "config.h"
#include "ssz_tick_time.h"
#include "drv_com.h"
#include "ssz_common.h"
#include "dev_def.h"
#include "event.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "sim_interface.h"
#endif
#include "infusion_motor.h"
#include "three_valve_motor.h"
#include "drv_infusion_motor.h"
#include "drv_stf_motor.h"
#include "mid_adc.h"
#include "drv_lowpower.h"
#ifdef SSZ_TARGET_MACHINE
#include "tim.h"
#endif
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
bool g_interrupt_is_confirm_key_pressed = false;

/************************************************
* Function 
************************************************/

/**
  * @brief  SYSTICK callback.
  * @retval None
  */
void HAL_SYSTICK_Callback(void)
{
	ssz_tick_time_increase(MSECS_PER_TICK);
	event_set_quick(kEventNewTick);
}


/**
* @brief  EXTI line detection callbacks.
* @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
* @retval None
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	int another_hall_value;

    if (GPIO_Pin == SZ_HALL_A_Pin && g_infusion_motor_state == kMotorRun) {//infusion motor encoder
		//check direction
		another_hall_value = ssz_gpio_is_high(SZ_HALL_B_GPIO_Port, SZ_HALL_B_Pin);
		//printf("%d\n",another_hall_value);
		if (g_infusion_motor_running_dir==kForward&& another_hall_value ==1) {
			g_infusion_motor_direction_error_count++;
			g_infusion_motor_a_b_direction_error_total ++ ;
		//	printf("DIR error[%d]\n", g_infusion_motor_direction_error_count);
		}
		else if(g_infusion_motor_running_dir == kBackward&& another_hall_value == 0){
			g_infusion_motor_direction_error_count++;
			g_infusion_motor_a_b_direction_error_total++;
		//	printf("DIR error[%d]\n", g_infusion_motor_direction_error_count);
		}
		else {
			//direction is right
			g_infusion_motor_direction_error_count = 0;

			//add encoder
			g_infusion_motor_encoder++;
		}
		//if direction error too much, need notify
		if (!g_infusion_motor_is_event_set &&
			g_infusion_motor_direction_error_count>=MOTOR_ALLOW_DIRECTION_ERROR_COUNT) {
			drv_infusion_motor_brake();
			g_infusion_motor_is_event_set = true;
			event_set(kEventInfusionMotorDirectionWrong);
		}


		if (g_infusion_motor_expect_encoder!=-1 &&
			g_infusion_motor_encoder>=g_infusion_motor_expect_encoder &&
			!g_infusion_motor_is_event_set)
		{
			drv_infusion_motor_brake();
			g_infusion_motor_is_event_set = true;
			event_set(kEventInfusionMotorFinishTarget);

		}

#if MOTOR_ENABLE_CALC_SPEED
		if (g_infusion_motor_is_speed_done == true)
		{
            g_infusion_motor_is_speed_done = false;
            g_infusion_motor_check_speed_encoder_start_time = ssz_tick_time_now();
            g_infusion_motor_check_speed_encoder=g_infusion_motor_encoder;
		}

        if(g_infusion_motor_encoder >= g_infusion_motor_check_speed_encoder){
    		if (g_infusion_motor_encoder-g_infusion_motor_check_speed_encoder >= MOTOR_SPEED_CHECK_COUNT)
    		{
                g_infusion_motor_is_speed_done = true;
                g_infusion_motor_check_speed_encoder_run_time=ssz_tick_time_elapsed(g_infusion_motor_check_speed_encoder_start_time);
                event_set(kEventInfusionMotorGetSpeedFinished);
    		}
        }else{
            if (INT32_MAX-g_infusion_motor_check_speed_encoder+g_infusion_motor_encoder+1 >= MOTOR_SPEED_CHECK_COUNT)
            {
                g_infusion_motor_is_speed_done = true;
                g_infusion_motor_check_speed_encoder_run_time=ssz_tick_time_elapsed(g_infusion_motor_check_speed_encoder_start_time); 
                event_set(kEventInfusionMotorGetSpeedFinished);
            }
        }
#endif
	}
#if MOTOR_ENABLE_CHECK_ENCODER_AFTER_STOP
	else if (GPIO_Pin == SZ_HALL_A_Pin ) {
		if (ssz_gpio_is_high(SZ_HALL_B_GPIO_Port, SZ_HALL_B_Pin)==0) {
			g_infusion_motor_forward_encoder_after_stop++;
		}
		else {
			g_infusion_motor_backward_encoder_after_stop++;
		}
	}
#endif

    if (GPIO_Pin == SZDJ_SUDU_Pin  && g_infusion_motor_state == kMotorRun) {//infusion motor optical_coupler encoder
		g_infusion_motor_optical_coupler_encoder ++;
        //printf("encoder= %d\n",g_infusion_motor_opticalcoupler_encoder);
		// infusion motor expected optical_coupler encoder is finished
		// and stop infusion motor is stop by optical_coupler
		if (g_infusion_motor_expect_optical_coupler_encoder!=-1 &&
			g_infusion_motor_optical_coupler_encoder>=g_infusion_motor_expect_optical_coupler_encoder &&
			!g_infusion_motor_is_event_set)
		{
			drv_infusion_motor_brake();
			g_infusion_motor_is_event_set = true;
			event_set(kEventInfusionMotorFinishTarget);

		}
    }


    if (GPIO_Pin == STF_HALL_A_Pin && g_three_valve_motor_state == kMotorRun) {//three-valve motor encoder
		//check direction
		another_hall_value = ssz_gpio_is_high(STF_HALL_B_GPIO_Port, STF_HALL_B_Pin);
		//printf("%d\n",another_hall_value);
		if (g_three_valve_motor_running_dir == kForward&& another_hall_value == 0) {
			g_three_valve_motor_direction_error_count ++;
			g_three_valve_motor_direction_error_total++;
			//printf("DIR Wrong %d\n",g_three_valve_motor_direction_error_count);
		}
		else if(g_three_valve_motor_running_dir == kBackward&& another_hall_value == 1){
			g_three_valve_motor_direction_error_count++;
			g_three_valve_motor_direction_error_total++;
			//printf("DIR Wrong %d\n",g_three_valve_motor_direction_error_count);
		}
		else {
			//direction is right
			g_three_valve_motor_direction_error_count = 0;

			//add encoder
			g_three_valve_motor_encoder ++;
		}
		//if direction error too much, need notify
		if (!g_three_valve_motor_is_event_set&&
			g_three_valve_motor_direction_error_count >= THREE_MOTOR_ALLOW_DIRECTION_ERROR_COUNT) {
			drv_three_valve_motor_brake();
			event_set(kEventThreeValveMotorDirectionWrong);
			g_three_valve_motor_is_event_set = true;
		}


		if (!g_three_valve_motor_is_event_set&&
			g_three_valve_motor_expect_encoder!=-1 &&
			g_three_valve_motor_encoder >= g_three_valve_motor_expect_encoder)
		{
			//printf("three valve current: %d\n",mid_adc_get_average_ADC_channel_value(kSTFDJCurrentADC));
			drv_three_valve_motor_brake();
			event_set(kEventThreeValveMotorFinishTarget);
			g_three_valve_motor_is_event_set = true;
		}
	}

	if (GPIO_Pin == KEY_CONFIRM_Pin){
		g_interrupt_is_confirm_key_pressed = true;
	}

}
#ifdef SSZ_TARGET_MACHINE
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim == &htim5){
		g_drv_lowpower_wake_up_by_time = true;
	}

}
#endif

