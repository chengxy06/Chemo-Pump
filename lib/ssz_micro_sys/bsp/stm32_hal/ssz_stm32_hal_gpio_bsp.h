#pragma once
#include "stdint.h"
/************************************************
* Declaration
************************************************/
#ifdef SSZ_TARGET_STM32F4XX_HAL
#include "stm32f4xx_hal.h"
#elif defined SSZ_TARGET_STM32F1XX_HAL
#include "stm32f1xx_hal.h"
#elif defined SSZ_TARGET_STM32L4XX_HAL
#include "stm32l4xx_hal.h"
#elif defined SSZ_TARGET_STM32L1XX_HAL
#include "stm32l1xx_hal.h"
#elif defined SSZ_TARGET_STM32L0XX_HAL
#include "stm32l0xx_hal.h"
#else
#error "target not support"
#endif

typedef unsigned short BSP_PIN;
typedef GPIO_TypeDef* BSP_GPIO;

#define BSP_GPIOA GPIOA
#define BSP_GPIOB GPIOB
#define BSP_GPIOC GPIOC
#define BSP_GPIOD GPIOD
#define BSP_GPIOE GPIOE
#define BSP_GPIOF GPIOF
#define BSP_GPIOG GPIOG
#define BSP_GPIOH GPIOH
#define BSP_GPIOI GPIOI
#define BSP_GPIOJ GPIOJ
#define BSP_GPIOK GPIOK

#define bsp_gpio_set(...) HAL_GPIO_WritePin(__VA_ARGS__, GPIO_PIN_SET)
#define bsp_gpio_quick_set(gpio,pin) (gpio->BSRR = pin)
#define bsp_gpio_clear(...) HAL_GPIO_WritePin(__VA_ARGS__, GPIO_PIN_RESET)
#define bsp_gpio_quick_clear(gpio,pin) (gpio->BSRR = (uint32_t)pin << 16U)
#define bsp_gpio_get(...) HAL_GPIO_ReadPin(__VA_ARGS__)
#define bsp_gpio_quick_get(gpio,pin) ((gpio->IDR & pin))