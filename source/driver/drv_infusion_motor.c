/************************************************
* DESCRIPTION:
* infusion motor driver
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-14 jlli
* Initial revision.
*
************************************************/
//#include "stm32l1xx_hal.h"
#include "dev_def.h"
#include "drv_infusion_motor.h"
#include "ssz_gpio.h"

#ifdef SSZ_TARGET_SIMULATOR
#include "sim_interface.h"
#endif

#ifdef SSZ_TARGET_MACHINE
#include "tim.h"	 
#endif

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
int g_infusion_motor_is_IN1_pin_as_PWM = false;  
#endif

/************************************************
* Function 
************************************************/

//enable infusion motor sleep
void drv_infusion_motor_sleep_enable()
{
  ssz_gpio_clear(SZ_MOTOR_SLEEP_GPIO_Port, SZ_MOTOR_SLEEP_Pin);
#ifdef SSZ_TARGET_SIMULATOR
  sim_timer_clear("infusion_motor");
  sim_timer_clear("infusion_motor_optical");
#endif
}

//disable infusion motor sleep
void drv_infusion_motor_sleep_disable()
{
  ssz_gpio_set(SZ_MOTOR_SLEEP_GPIO_Port, SZ_MOTOR_SLEEP_Pin);
#ifdef SSZ_TARGET_SIMULATOR
  sim_timer_set("infusion_motor", 20, 1);
  sim_timer_set("infusion_motor_optical", 100, 1);
#endif
}
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
void drv_infusion_motor_change_IN1_as_GPIO(){
	if (g_infusion_motor_is_IN1_pin_as_PWM) {
		if(htim3.Instance ){
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
			HAL_TIM_PWM_DeInit(&htim3);
		}		

		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = SZ_MOTOR_IN1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(SZ_MOTOR_IN1_GPIO_Port, &GPIO_InitStruct);
		g_infusion_motor_is_IN1_pin_as_PWM = false;
	}
}
void drv_infusion_motor_change_IN1_as_PWM(){
	if (!g_infusion_motor_is_IN1_pin_as_PWM) {

        __HAL_RCC_TIM3_CLK_ENABLE();

		MX_TIM3_Init_ex(1); 
   		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		g_infusion_motor_is_IN1_pin_as_PWM = true;
	}
}

#endif

//set infusion motor move forward
void drv_infusion_motor_move_forward(int duty_cycle)
{
    drv_three_valve_motor_init();

#if INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
   drv_infusion_motor_change_IN1_as_PWM();
   TIM3->CCR1  = duty_cycle ;
#else
   ssz_gpio_clear(SZ_MOTOR_IN1_GPIO_Port, SZ_MOTOR_IN1_Pin);
#endif   
   ssz_gpio_set(SZ_MOTOR_IN2_GPIO_Port, SZ_MOTOR_IN2_Pin);
}

//set infusion motor move forward
void drv_infusion_motor_move_forward_ex()
{
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	if (g_infusion_motor_is_IN1_pin_as_PWM) {
		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
		HAL_TIM_PWM_MspDeInit(&htim3);

		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = SZ_MOTOR_IN1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
		g_infusion_motor_is_IN1_pin_as_PWM = false;
        __HAL_RCC_TIM3_CLK_DISABLE();
	}
#endif 
  ssz_gpio_clear(SZ_MOTOR_IN1_GPIO_Port, SZ_MOTOR_IN1_Pin);
  ssz_gpio_set(SZ_MOTOR_IN2_GPIO_Port, SZ_MOTOR_IN2_Pin);
  
}

//set infusion motor move backward
void drv_infusion_motor_move_backward(int duty_cycle)
{
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	drv_infusion_motor_change_IN1_as_GPIO();
#endif	
	ssz_gpio_set(SZ_MOTOR_IN1_GPIO_Port, SZ_MOTOR_IN1_Pin);
	ssz_gpio_clear(SZ_MOTOR_IN2_GPIO_Port, SZ_MOTOR_IN2_Pin);
}

//set infusion motor brake
void drv_infusion_motor_brake()
{
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	drv_infusion_motor_change_IN1_as_GPIO(); 
#endif
	ssz_gpio_set(SZ_MOTOR_IN1_GPIO_Port, SZ_MOTOR_IN1_Pin);
	ssz_gpio_set(SZ_MOTOR_IN2_GPIO_Port, SZ_MOTOR_IN2_Pin);

}

//set infusion motor continue inertially until stop
void drv_infusion_motor_inertial_stop()
{
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	drv_infusion_motor_change_IN1_as_GPIO();
#endif
	ssz_gpio_clear(SZ_MOTOR_IN1_GPIO_Port, SZ_MOTOR_IN1_Pin);
	ssz_gpio_clear(SZ_MOTOR_IN2_GPIO_Port, SZ_MOTOR_IN2_Pin);
}
void drv_infusion_motor_change_duty_cycle(int duty_cycle)
{
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	TIM3->CCR1	= duty_cycle ;
#endif   
}

void drv_infusion_motor_init()
{
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	g_infusion_motor_is_IN1_pin_as_PWM = true;	
	drv_infusion_motor_inertial_stop();
#endif	
}

