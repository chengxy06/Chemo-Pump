#include "ssz_config.h"
#if defined SSZ_TARGET_STM32L0XX_HAL || defined SSZ_TARGET_STM32L1XX_HAL || defined SSZ_TARGET_STM32F4XX_HAL||defined SSZ_TARGET_STM32F1XX_HAL
#include "ssz_common.h"
#include "ssz_gpio.h"
#include "ssz_i2c.h"
#include "ssz_spi.h"
#include "ssz_uart.h"
/************************************************
* Declaration
************************************************/
#define OPERATION_MAX_TIME 1000

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
//CPU run hz
int32_t ssz_system_clock() {
	return SystemCoreClock;
}
//system reset
void ssz_system_reset() {
	HAL_NVIC_SystemReset();
}
//delay microsecond
void ssz_delay_us(int us){
	uint32_t counter;
	
    /* Compute number of CPU cycles to wait for */
    counter = ((us+4)/5 * (SystemCoreClock / 1000000U));
    do{
      ssz_no_operation();
    }while(counter--);
}

//delay millisecond
void ssz_delay_ms(int ms){
	//get current time
	int32_t curr_ms = ssz_tick_time_now();

	//calc the elapse time is reach the specific time, if reach then stop delay
	while (ssz_tick_time_elapsed(curr_ms) < ms){
		ssz_sleep();
	}
}
//delay millisecond and clear watchdog to let the watchdog not effect
void ssz_delay_ms_with_clear_watchdog(int ms) {
	//get current time
	int32_t curr_ms = ssz_tick_time_now();

	//calc the elapse time is reach the specific time, if reach then stop delay
	while (ssz_tick_time_elapsed(curr_ms) < ms) {
		ssz_sleep();
		ssz_clear_watchdog();
	}
}
//sleep
void ssz_sleep() {
	__WFI();
}

#ifdef HAL_GPIO_MODULE_ENABLED
void ssz_gpio_set_mode(SszGpio gpio, SszPin pin_mask, SszGpioMode mode){
	GPIO_InitTypeDef GPIO_InitStruct;

	ssz_mem_zero(&GPIO_InitStruct, sizeof(GPIO_InitStruct));
	GPIO_InitStruct.Pin = pin_mask;
	switch (mode){
		case kSszGpioInputFloating:
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			break;
		case kSszGpioInputPD:
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			break;
		case kSszGpioInputPU:
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			break;
		case kSszGpioOutOD:
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			break;
		case kSszGpioOutOD_PD:
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			break;
		case kSszGpioOutOD_PU:
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			break;
		case kSszGpioOutPP:
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			break;
		case kSszGpioOutPP_PD:
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			break;
		case kSszGpioOutPP_PU:
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			break;			
					
		default:
		ssz_assert_fail();
		break;
	}

	HAL_GPIO_Init(gpio, &GPIO_InitStruct);
}
//enable clock
void ssz_gpio_enable_clock(SszGpio gpio){
	if (gpio == GPIOA){
		__HAL_RCC_GPIOA_CLK_ENABLE();
	}else if(gpio == GPIOB){
		__HAL_RCC_GPIOB_CLK_ENABLE();
	}else if(gpio == GPIOC){
		__HAL_RCC_GPIOC_CLK_ENABLE();
	}else if(gpio == GPIOD){
		__HAL_RCC_GPIOD_CLK_ENABLE();
	}else if(gpio == GPIOE){
		__HAL_RCC_GPIOE_CLK_ENABLE();
	//}else if(gpio == GPIOF){
	//	__HAL_RCC_GPIOF_CLK_ENABLE();
	}else{
		ssz_assert_fail();
	}
}

#endif

#ifdef HAL_I2C_MODULE_ENABLED
//return the read byte count
bool ssz_i2c_master_read(SszI2C* i2c, void *buff, int buff_size){
	if(HAL_I2C_Master_Receive((I2C_HandleTypeDef*)i2c->obj, 
		i2c->slave_address, buff, buff_size, OPERATION_MAX_TIME) != HAL_OK){
		return false;
	}

	return true;
}
//return the write byte count
bool ssz_i2c_master_write(SszI2C* i2c, const void *buff, int buff_size){
	if(HAL_I2C_Master_Transmit((I2C_HandleTypeDef*)i2c->obj, 
		i2c->slave_address, (uint8_t *)buff, buff_size, OPERATION_MAX_TIME) != HAL_OK){
		return false;
	}

	return true;
}
#endif

#ifdef HAL_SPI_MODULE_ENABLED
//return the read byte count
bool ssz_spi_read(SszSPI* obj, void *buff, int buff_size){
	if(HAL_SPI_Receive((SPI_HandleTypeDef*)obj->obj, 
		buff, buff_size, OPERATION_MAX_TIME) != HAL_OK){
		return false;
	}

	return true;
}
//return the write byte count
bool ssz_spi_write(SszSPI* obj, const void *buff, int buff_size){
	if(HAL_SPI_Transmit((SPI_HandleTypeDef*)obj->obj, 
		 (uint8_t *)buff, buff_size, OPERATION_MAX_TIME) != HAL_OK){
		return false;
	}

	return true;
}
void ssz_spi_wait_write_finish(SszSPI* obj) {

}
#endif	

#if defined HAL_UART_MODULE_ENABLED || HAL_USART_MODULE_ENABLED
//return the read byte count
bool ssz_uart_read(SszUART* obj, void *buff, int buff_size)
{
	bool ret = false;
	if (obj->is_enable_synchronous) {
#if defined HAL_USART_MODULE_ENABLED
		if (HAL_USART_Receive((UART_HandleTypeDef*)obj->obj,
			(uint8_t*)buff, buff_size, OPERATION_MAX_TIME) == HAL_OK) {
			ret = true;
		}
#endif
	}
	else {
#if defined HAL_UART_MODULE_ENABLED
		if (HAL_UART_Receive((UART_HandleTypeDef*)obj->obj,
			(uint8_t*)buff, buff_size, OPERATION_MAX_TIME) == HAL_OK) {
			ret = true;
		}
#endif
	}

	return ret;
}
//return the write byte count
bool ssz_uart_write(SszUART* obj, const void *buff, int buff_size)
{
	bool ret = false;
	if (obj->is_enable_synchronous) {
#if defined HAL_USART_MODULE_ENABLED
		if (HAL_USART_Transmit((UART_HandleTypeDef*)obj->obj,
			(uint8_t*)buff, buff_size, OPERATION_MAX_TIME) != HAL_OK) {
			ret = true;
		}
#endif
	}
	else {
#if defined HAL_UART_MODULE_ENABLED
		if (HAL_UART_Transmit((UART_HandleTypeDef*)obj->obj,
			(uint8_t*)buff, buff_size, OPERATION_MAX_TIME) != HAL_OK) {
			ret = true;
		}
#endif
	}
    
    return ret;
}
#endif



#endif

