/************************************************
* DESCRIPTION:
*   three-valve motor diver
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
#include "drv_stf_motor.h"
#ifdef SSZ_TARGET_MACHINE
#include "tim.h"	 
#endif

/************************************************
* Declaration
************************************************/
#if	THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
int g_three_valve_motor_is_IN1_pin_as_PWM = false; 
int g_three_valve_motor_is_IN2_pin_as_PWM = false;
#endif

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
#if	THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
void drv_three_valve_motor_change_IN1_as_GPIO(){
	if (g_three_valve_motor_is_IN1_pin_as_PWM) {
		if(htim10.Instance ){
			HAL_TIM_PWM_Stop(&htim10, TIM_CHANNEL_1);
			HAL_TIM_PWM_DeInit(&htim10);
		}
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = STF_MOTOR_IN1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(STF_MOTOR_IN1_GPIO_Port, &GPIO_InitStruct);
		g_three_valve_motor_is_IN1_pin_as_PWM = false;
	}
}
void drv_three_valve_motor_change_IN1_as_PWM(){
	if (!g_three_valve_motor_is_IN1_pin_as_PWM) {
		MX_TIM10_Init(); 
   		HAL_TIM_PWM_Start(&htim10,TIM_CHANNEL_1);
		g_three_valve_motor_is_IN1_pin_as_PWM = true;
	}
}
void drv_three_valve_motor_change_IN2_as_GPIO(){
	if (g_three_valve_motor_is_IN2_pin_as_PWM) {
		if(htim11.Instance ){
			HAL_TIM_PWM_Stop(&htim11, TIM_CHANNEL_1);
			HAL_TIM_PWM_DeInit(&htim11);
		}		

		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.Pin = STF_MOTOR_IN2_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(STF_MOTOR_IN2_GPIO_Port, &GPIO_InitStruct);
		g_three_valve_motor_is_IN2_pin_as_PWM = false;
	}
}
void drv_three_valve_motor_change_IN2_as_PWM(){
	if (!g_three_valve_motor_is_IN2_pin_as_PWM) {
		MX_TIM11_Init(); 
   		HAL_TIM_PWM_Start(&htim11,TIM_CHANNEL_1);
		g_three_valve_motor_is_IN2_pin_as_PWM = true;
	}
}
#endif

//enable all motor power supply form system power supply
void drv_all_motor_pwr_enable()
{
  ssz_gpio_set(ALL_MOTOR_PWR_EN_GPIO_Port, ALL_MOTOR_PWR_EN_Pin);
}

//disable all motor power supply form system power supply
void drv_all_motor_pwr_disable()
{
  ssz_gpio_clear(ALL_MOTOR_PWR_EN_GPIO_Port, ALL_MOTOR_PWR_EN_Pin);
}

//enable all 5v power supply 
void drv_all_5v_pwr_enable()
{
  ssz_gpio_set(ALL_5V_PWR_EN_GPIO_Port, ALL_5V_PWR_EN_Pin);
}

//disable all 5v power supply 
void drv_all_5v_pwr_disable()
{
  ssz_gpio_clear(ALL_5V_PWR_EN_GPIO_Port, ALL_5V_PWR_EN_Pin);
}

//enable three-valve motor 5v power supply
void drv_three_valve_motor_pwr_enable()
{
  ssz_gpio_set(STF_5V_IN_EN_GPIO_Port, STF_5V_IN_EN_Pin);
}

//disable three-valve motor 5v power supply 
void drv_three_valve_motor_pwr_disable()
{
  ssz_gpio_clear(STF_5V_IN_EN_GPIO_Port, STF_5V_IN_EN_Pin);
}

//enable three-valve motor sleep
void drv_three_valve_motor_sleep_enable()
{
  ssz_gpio_clear(STF_MOTOR_SLEEP_GPIO_Port, STF_MOTOR_SLEEP_Pin);
}

//disable three-valve motor sleep
void drv_three_valve_motor_sleep_disable()
{
  ssz_gpio_set(STF_MOTOR_SLEEP_GPIO_Port, STF_MOTOR_SLEEP_Pin);
}

//set three-valve motor move forward
void drv_three_valve_motor_move_forward(int duty_cycle)
{
#if THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	drv_three_valve_motor_change_IN1_as_PWM(); 
	drv_three_valve_motor_change_IN2_as_GPIO(); 
	TIM10->CCR1	= duty_cycle ;
#else
	ssz_gpio_set(STF_MOTOR_IN1_GPIO_Port, STF_MOTOR_IN1_Pin);
#endif  
	ssz_gpio_clear(STF_MOTOR_IN2_GPIO_Port, STF_MOTOR_IN2_Pin);
}
//set three-valve motor move backward
void drv_three_valve_motor_move_backward(int duty_cycle)
{
#if THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	drv_three_valve_motor_change_IN1_as_GPIO(); 
	drv_three_valve_motor_change_IN2_as_PWM(); 
	TIM11->CCR1	= duty_cycle ;
#else
	ssz_gpio_set(STF_MOTOR_IN2_GPIO_Port, STF_MOTOR_IN2_Pin);
#endif  
	ssz_gpio_clear(STF_MOTOR_IN1_GPIO_Port, STF_MOTOR_IN1_Pin);
}

//set three-valve motor move forward on gpio mode
void drv_three_valve_motor_move_forward_ex()
{
#if THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	drv_three_valve_motor_change_IN1_as_GPIO(); 
	drv_three_valve_motor_change_IN2_as_GPIO(); 
#endif  
  ssz_gpio_set(STF_MOTOR_IN1_GPIO_Port, STF_MOTOR_IN1_Pin);
  ssz_gpio_clear(STF_MOTOR_IN2_GPIO_Port, STF_MOTOR_IN2_Pin);
}
//set three-valve motor move backward on gpio mode
void drv_three_valve_motor_move_backward_ex()
{
#if THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	drv_three_valve_motor_change_IN1_as_GPIO(); 
	drv_three_valve_motor_change_IN2_as_GPIO();   
#endif  
	ssz_gpio_set(STF_MOTOR_IN2_GPIO_Port, STF_MOTOR_IN2_Pin);  		
	ssz_gpio_clear(STF_MOTOR_IN1_GPIO_Port, STF_MOTOR_IN1_Pin);
}

//set three-valve motor brake
void drv_three_valve_motor_brake()
{
#if THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	drv_three_valve_motor_change_IN1_as_GPIO();
	drv_three_valve_motor_change_IN2_as_GPIO();
#endif 	
	ssz_gpio_set(STF_MOTOR_IN1_GPIO_Port, STF_MOTOR_IN1_Pin);
	ssz_gpio_set(STF_MOTOR_IN2_GPIO_Port, STF_MOTOR_IN2_Pin);
}
//set three-valve motor continue inertially until stop
void drv_three_valve_motor_inertial_stop()
{
#if THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	drv_three_valve_motor_change_IN1_as_GPIO();
	drv_three_valve_motor_change_IN2_as_GPIO();
#endif 	
	ssz_gpio_clear(STF_MOTOR_IN1_GPIO_Port, STF_MOTOR_IN1_Pin);
	ssz_gpio_clear(STF_MOTOR_IN2_GPIO_Port, STF_MOTOR_IN2_Pin);
}
 
// three-vavle position detection power enable
void drv_three_valve_pos_detect_pwr_enable()
{
  ssz_gpio_set(STF_POS_DETECT_PWR_EN_GPIO_Port, STF_POS_DETECT_PWR_EN_Pin);
}

// three-vavle position detection power disable
void drv_three_valve_pos_detect_pwr_disable()
{
  ssz_gpio_clear(STF_POS_DETECT_PWR_EN_GPIO_Port, STF_POS_DETECT_PWR_EN_Pin);
}

// check if three-vavle has reached position B
bool drv_three_valve_is_pos_a_detected()
{
    bool ret;
    if(ssz_gpio_is_high(STF_POS_DETECT_B_GPIO_Port,STF_POS_DETECT_B_Pin))
        ret = false;
    else
        ret = true;
    return ret;
}

// check if three-vavle has reached position A
bool drv_three_valve_is_pos_b_detected()
{
    bool ret;
    if(ssz_gpio_is_high(STF_POS_DETECT_A_GPIO_Port,STF_POS_DETECT_A_Pin))
        ret = false;
    else
        ret = true;
    return ret;
}

void drv_three_valve_motor_change_duty_cycle_to_a(int duty_cycle)
{
#if	THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1 
	TIM11->CCR1 = duty_cycle ;
#endif	
}
void drv_three_valve_motor_change_duty_cycle_to_b(int duty_cycle)
{
#if	THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1 	
	TIM10->CCR1 = duty_cycle ;
#endif	
}

void drv_three_valve_motor_init()
{
#if	THREE_VALVE_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	g_three_valve_motor_is_IN1_pin_as_PWM = true;
	g_three_valve_motor_is_IN2_pin_as_PWM = true;
	drv_three_valve_motor_inertial_stop();
#endif	
}

