/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0  2017-6-26 jcwu
* Initial revision.
*
************************************************/
#include "drv_lowpower.h"
#include "drv_isd2360.h"
#include "drv_stf_motor.h"
#include "ssz_spi.h"
#include "ssz_uart.h"
#include "ssz_common.h"
#include "ssz_utility.h"
#include "dev_def.h"
#include "drv_oled.h"
#include "init.h"
#include "pill_box_install_detect.h"
#include "drv_stf_motor.h"
#include "pressure_bubble_sensor.h"
#include "mid_adc.h"
#include "drv_sst25_flash.h"
#include "ssz_sim_i2c.h"
#include "ssz_common.h"
#include "vibrater_motor.h"
#include "main.h"
#include "init.h"
#include "ssz_locale.h"
#include "vibrater_motor.h"
#include "drv_infusion_motor.h"
#include "sys_power.h"
#include "display.h"
#include "app_mcu_comm.h"
#include "voice.h"

#ifdef SSZ_TARGET_MACHINE
#include "stm32l1xx_hal_tim.h"
#include "tim.h"
#endif
#include "init.h"
#include "led.h"
/************************************************
* Declaration
************************************************/
#define LOW_POWER_DEBUG_ENABLE 0

static void APBH_Config_low_sleep(void) ;
#ifdef SSZ_TARGET_MACHINE
void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle);
void drv_rtc_init(void);
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void drv_com_init(void);
void drv_lowpower_gpio_set_to_lowest_current( void );
void init_driver();
#endif
/************************************************
* Variable 
************************************************/
bool g_drv_lowpower_wake_up_by_time ;
bool g_drv_lowpower_is_slaver_wake_up;
bool g_drv_lowpower_is_need_hold_led_and_voice;

/************************************************
* Function 
************************************************/
void drv_bt_pwr_enable()
{
  ssz_gpio_set(BT_PWR_EN_GPIO_Port, BT_PWR_EN_Pin);
}

void drv_bt_pwr_disable(void)
{
  ssz_gpio_clear(BT_PWR_EN_GPIO_Port, BT_PWR_EN_Pin);
}

void drv_rtc_lowest_pwr_mode(void)
{
	ssz_gpio_set(RTC_I2C_SDA_GPIO_Port, RTC_I2C_SDA_Pin);
	ssz_gpio_set(RTC_I2C_SCK_GPIO_Port, RTC_I2C_SCK_Pin);  
}

void drv_all_led_off(void)
{
	ssz_gpio_clear(LED_Y_GPIO_Port, LED_Y_Pin);
	ssz_gpio_clear(LED_G_GPIO_Port, LED_G_Pin);
	ssz_gpio_clear(LED_R_GPIO_Port, LED_R_Pin);
}
 
void drv_all_led_on( void)
{
	ssz_gpio_set(LED_Y_GPIO_Port, LED_Y_Pin);
	ssz_gpio_set(LED_G_GPIO_Port, LED_G_Pin);
	ssz_gpio_set(LED_R_GPIO_Port, LED_R_Pin);
} 
void drv_enter_low_power_colse_clock( void )
{ 
#ifdef SSZ_TARGET_MACHINE 
 __HAL_RCC_TIM2_CLK_DISABLE();      
 __HAL_RCC_TIM3_CLK_DISABLE();      
 __HAL_RCC_TIM4_CLK_DISABLE();      
 __HAL_RCC_TIM6_CLK_DISABLE();      
 __HAL_RCC_TIM7_CLK_DISABLE();      
 __HAL_RCC_WWDG_CLK_DISABLE();      
 __HAL_RCC_SPI2_CLK_DISABLE();      
 __HAL_RCC_USART2_CLK_DISABLE();    
 __HAL_RCC_USART3_CLK_DISABLE();    
 __HAL_RCC_I2C1_CLK_DISABLE();      
 __HAL_RCC_I2C2_CLK_DISABLE();      
 __HAL_RCC_USB_CLK_DISABLE();       
 __HAL_RCC_PWR_CLK_DISABLE();       
 __HAL_RCC_DAC_CLK_DISABLE();       
 __HAL_RCC_COMP_CLK_DISABLE();      
 __HAL_RCC_CRC_CLK_DISABLE();       
 __HAL_RCC_FLITF_CLK_DISABLE();     
 __HAL_RCC_DMA1_CLK_DISABLE();      

 __HAL_RCC_SYSCFG_CLK_DISABLE();    
 __HAL_RCC_TIM9_CLK_DISABLE();      
 __HAL_RCC_TIM10_CLK_DISABLE();     
 __HAL_RCC_TIM11_CLK_DISABLE();     
 __HAL_RCC_ADC1_CLK_DISABLE();      
 __HAL_RCC_SPI1_CLK_DISABLE();      
 __HAL_RCC_USART1_CLK_DISABLE();   
#endif
}

void drv_lowpower_gpio_set_to_lowest_current(void)
{
	drv_rtc_lowest_pwr_mode();
	drv_infusion_motor_sleep_enable();
}
void drv_lowpower_pcb_peripheral_power_disable(  void )
{
// sys power ad 
	sys_power_ADC_disable();	
// SYS_PWR
	sys_power_enable();

// 3.3V  5.0V  3.3V_MOTOR  VREF_3V3  power close     6.0V  by slaver control
	sys_master_mcu_power_disable();    // 3.3V---> SYS_PWR by  diode
	drv_all_5v_pwr_disable();
	drv_all_motor_pwr_disable(); // close 3.3V_MOTOR power  
	mia_adc_vref_disable();      // VREF_3V3
//	peripheral  power	
	pressure_and_bubble_sensor_pwr_disable();
	drv_three_valve_pos_detect_pwr_disable();
	pill_box_install_sensor_left_pwr_disable();	
	pill_box_install_sensor_right_pwr_disable();
	drv_bt_pwr_disable();
	display_close() ;  // OLED   power down  
	drv_sst25_pcb_sleep_enable( );
	if(!g_drv_lowpower_is_need_hold_led_and_voice){
		drv_isd2360_pcb_power_disable();   
	}
	drv_three_valve_motor_pwr_disable();	
	if(!g_drv_lowpower_is_need_hold_led_and_voice){
		drv_all_led_off();	
	}
	vibrator_motor_stop();
	drv_oled_deinit();
	
	drv_lowpower_gpio_set_to_lowest_current();
}

void drv_lowpower_pcb_peripheral_power_enable( void ){
	drv_three_valve_pos_detect_pwr_enable();
	pill_box_install_sensor_left_pwr_enable();	
	pill_box_install_sensor_right_pwr_enable();
	drv_bt_pwr_enable();
	drv_oled_pwr_en(); // OLED   power on 
	drv_sst25_pcb_sleep_disable( );
	mid_adc_vref_enable();
	drv_all_motor_pwr_enable();
	sys_power_enable();
	sys_master_mcu_power_enable();
	drv_all_5v_pwr_enable();
}

void drv_config_gpio_for_low_power(){
#ifdef SSZ_TARGET_MACHINE
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();

	if(g_drv_lowpower_is_need_hold_led_and_voice){
		GPIO_InitStructure.Pin = KEY_LEFT_Pin|KEY_RIGHT_Pin |KEY_BACK_Pin
		|GPIO_PIN_0|AC_CONNECT_Pin|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8
		|GPIO_PIN_12|GPIO_PIN_14;
	}else{
		GPIO_InitStructure.Pin = KEY_LEFT_Pin|KEY_RIGHT_Pin |KEY_BACK_Pin
		|GPIO_PIN_0|AC_CONNECT_Pin|GPIO_PIN_1|GPIO_PIN_7|GPIO_PIN_8
		|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;	
	}
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.Pin =	KEY_CONFIRM_Pin;
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
	__HAL_GPIO_EXTI_CLEAR_IT(KEY_CONFIRM_Pin);  //  ok

	GPIO_InitStructure.Pin = GPIO_PIN_9;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL ;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);
	

#if LOW_POWER_DEBUG_ENABLE
	GPIO_InitStructure.Pin = GPIO_PIN_All &(~(GPIO_PIN_13|GPIO_PIN_14));
#else
	GPIO_InitStructure.Pin = GPIO_PIN_All;
#endif
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 

	GPIO_InitStructure.Pin = GPIO_PIN_All;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
	HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
	
	if(g_drv_lowpower_is_need_hold_led_and_voice){
		GPIO_InitStructure.Pin = GPIO_PIN_All&(~VOICE_PWR_EN_Pin);
	}
	else{
		GPIO_InitStructure.Pin = GPIO_PIN_All;
	}
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Disable GPIOs clock */
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
	__HAL_RCC_GPIOE_CLK_DISABLE();
	__HAL_RCC_GPIOH_CLK_DISABLE();

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);	 		
  	NVIC_DisableIRQ(EXTI3_IRQn);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
#endif
}
void drv_lowpower_sleep_slaver(){
	//the slaver will sleep when the pin is low, so need set it
	sys_power_wake_up_slaver_pin_low();
	app_mcu_send_to_slave(COMM_SLAVER_MCU_SLEEP,0,0);
}
void drv_lowpower_wakeup_slaver(){
	sys_power_wake_up_slaver_pin_high();
}

// return: true->exit by time out, false->exit by other interrupt
bool drv_lowpower_enter_lpsleep(  int data, bool is_need_hold_led_and_voice )
{  		
#ifdef	SSZ_TARGET_MACHINE      //low sleep SSZ_TARGET_MACHINE  jcwu_TARGET_MACHINE	

	g_drv_lowpower_is_need_hold_led_and_voice = is_need_hold_led_and_voice;
	g_drv_lowpower_is_slaver_wake_up = false;

	display_close() ;//it use spi to sleep, so need call before spi deinit
	if(!is_need_hold_led_and_voice){
		voice_device_close();
	}
	HAL_ADC_MspDeInit(&hadc);

	HAL_RCC_DeInit();
	SystemCoreClockUpdate();
	HAL_Init();		
	//HAL_SuspendTick( );	
	// mcu peripheral
	ssz_check(HAL_SPI_DeInit(&hspi1)==HAL_OK); 
 	ssz_check(HAL_SPI_DeInit(&hspi2)==HAL_OK);	
 	ssz_check(HAL_SPI_DeInit(&hspi3)==HAL_OK);	
	
	ssz_check(HAL_UART_DeInit(&huart1)==HAL_OK);
	ssz_check(HAL_UART_DeInit(&huart2 )==HAL_OK);
	ssz_check(HAL_UART_DeInit(&huart3 )==HAL_OK);
		
	ssz_check(HAL_ADC_DeInit(&hadc)==HAL_OK);

	ssz_check(HAL_TIM_Base_DeInit(&htim5)==HAL_OK);
	
   	__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
   	__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
   	__HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);	
	
//	APBH_Config_low_sleep( );
    //HAL_SuspendTick( );	

	// set io state
	drv_lowpower_pcb_peripheral_power_disable();	

// Disable Prefetch Buffer   
  	HAL_PWR_DisablePVD();
	__HAL_FLASH_PREFETCH_BUFFER_DISABLE();
	// Enable Ultra low power mode 
	HAL_PWREx_EnableUltraLowPower();
	// Enable the fast wake up from Ultra low power mode  
	HAL_PWREx_EnableFastWakeUp();	

	drv_config_gpio_for_low_power();
	
	drv_enter_low_power_colse_clock();
			
	g_drv_lowpower_wake_up_by_time = false;
	if( data > 0){
		//根据131.072Khz的频率来设置timer的参数, 后面的AHB必须变成这个频率才能准确
		MX_TIM5_Init_ex(data);	
		__HAL_TIM_CLEAR_IT(&htim5, TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim5);	
	}

	APBH_Config_low_sleep( ); //
	HAL_SuspendTick( );	
#if !LOW_POWER_DEBUG_ENABLE	
	// Enable the power down mode during Sleep mode 
	__HAL_FLASH_SLEEP_POWERDOWN_ENABLE();
	HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#else
	__WFI();
#endif	
  	
//	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

	if( data > 0){
		HAL_TIM_Base_Stop_IT(&htim5);	
	}

#endif		
	return g_drv_lowpower_wake_up_by_time;
}
void drv_lowpower_exit_lpsleep(  void )
{ 
	//int wake_up_slaver_delay = 40 ;
#ifdef SSZ_TARGET_MACHINE
	
	SET_BIT(PWR->CR, PWR_CR_CWUF); // Clear Wakeup Flag

 	HAL_RCC_DeInit();
	HAL_Init();	
	SystemClock_Config();	
 	MX_GPIO_Init();
	MX_ADC_Init();
 	MX_SPI1_Init();
 	MX_SPI2_Init();
 	MX_SPI3_Init();
  	MX_USART1_UART_Init();
  	MX_USART2_UART_Init();
  	MX_USART3_UART_Init();	
	
	drv_com_init();

	led_resume();

	drv_three_valve_pos_detect_pwr_enable();//drv_three_valve_pos_detect_pwr_disable();
	pill_box_install_sensor_left_pwr_enable();	
	pill_box_install_sensor_right_pwr_enable();
	drv_bt_pwr_disable();
	drv_oled_pwr_dis();
	drv_sst25_pcb_sleep_disable( );
	mid_adc_vref_enable();
	drv_all_motor_pwr_enable();
	sys_power_enable();
	drv_all_5v_pwr_enable();
	drv_three_valve_motor_pwr_disable();	
	pressure_and_bubble_sensor_pwr_disable();
	dev_init();
	drv_rtc_init();
  	//drv_oled_init();
	//drv_oled_cs_dis();
	drv_sst25_flash_init();
	//drv_isd2360_initial();	
	drv_infusion_motor_init();
	drv_three_valve_motor_init();		
	if(!g_drv_lowpower_is_need_hold_led_and_voice){// 关闭语音电源  
		drv_isd2360_deinitial();
	}	

#endif
}

#ifdef SSZ_TARGET_MACHINE
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = MSI
  *            MSI Range                      = 0
  *            SYSCLK(Hz)                     = 64000
  *            HCLK(Hz)                       = 32000
  *            AHB Prescaler                  = 2
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            Main regulator output voltage  = Scale2 mode
  * @param  None
  * @retval None
  */
static void APBH_Config_low_sleep(void) 
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable Power Control clock */
   __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2); 
  /* For STM32L1: Low power sleep mode can only be entered when VCORE is in range 2 */
//  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
  /* Enable MSI Oscillator */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_2; // Set temporary MSI range  
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

/* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};
  
//  HAL_RCC_GetClockConfig(&RCC_OscInitStruct);
  /* Select MSI as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType       = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | 
RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV2; // RCC_SYSCLK_DIV2
  RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV1;  // APB1  131.072kHZ 
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Note: For STM32L1, to enable low power sleep mode, the system frequency  */
  /* should not exceed f_MSI range1.                                          */
  /* Set MSI range to 0 */
  __HAL_RCC_MSI_RANGE_CONFIG(RCC_MSIRANGE_2);
}
#endif

