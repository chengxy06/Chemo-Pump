/************************************************
* DESCRIPTION:
* 	define all used device, e.g. IO,SPI,I2C,ADC,UART
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-09 xqzhao
* Initial revision.
*
************************************************/
#pragma once 
#include "ssz_gpio.h" //have all GPIO
#include "ssz_spi.h"
#include "ssz_sim_i2c.h"
#include "ssz_uart.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "ssz_simulator_dev_def.h" //have all spi,i2c,uart...
#include "machine_sim_dev_map.h"
#endif

/************************************************
* Declaration
************************************************/

//define IO
#ifdef SSZ_TARGET_MACHINE
#include "main.h"
#else
#include "../../bsp/bsp_machine/Inc/main.h" //use machine's IO define
#endif

//define ADC
#ifdef SSZ_TARGET_MACHINE
#include "adc.h"
#define ADC_COM hadc

#else

#define ADC_COM ssz_sim_adc
#endif

//define SPI
#ifdef SSZ_TARGET_MACHINE
#include "spi.h"
#define FLASH_SPI hspi3
#define ISD2360_VOICE_SPK hspi1
#define OLED_SPI hspi2
#else
#define FLASH_SPI ssz_sim_spi1
#define ISD2360_VOICE_SPK ssz_sim_spi3
#define OLED_SPI ssz_sim_spi2
#endif


//define UART
#ifdef SSZ_TARGET_MACHINE
#include "usart.h"
#define PC_UART huart1
#define BT_UART huart3
#define MCU_UART huart2
#else
#define PC_UART ssz_sim_uart1
#define BT_UART ssz_sim_uart2
#define MCU_UART ssz_sim_uart3
#endif

//define UART
#ifdef SSZ_TARGET_MACHINE
#include "tim.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif
void dev_init();
#ifdef __cplusplus
}
#endif
