#pragma once
#include "ssz_config.h"
#include "ssz_def.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "../bsp/simulator/ssz_simulator_gpio_bsp.h"
#elif defined SSZ_TARGET_STM32F4XX_HAL || \
	defined SSZ_TARGET_STM32F1XX_HAL || \
	defined SSZ_TARGET_STM32L4XX_HAL || \
	defined SSZ_TARGET_STM32L1XX_HAL || \
	defined SSZ_TARGET_STM32L0XX_HAL

#include "../bsp/stm32_hal/ssz_stm32_hal_gpio_bsp.h"
#else
#error "target not support"
#endif
/************************************************
* Declaration
************************************************/
#define ssz_no_operation() __NOP()

//GPIO type define
#define SszGpio BSP_GPIO
//GPIO pin define
#define SSZ_GPIOA BSP_GPIOA
#define SSZ_GPIOB BSP_GPIOB
#define SSZ_GPIOC BSP_GPIOC
#define SSZ_GPIOD BSP_GPIOD
#define SSZ_GPIOE BSP_GPIOE
#define SSZ_GPIOF BSP_GPIOF
#define SSZ_GPIOG BSP_GPIOG
#define SSZ_GPIOH BSP_GPIOH
#define SSZ_GPIOI BSP_GPIOI
#define SSZ_GPIOJ BSP_GPIOJ
#define SSZ_GPIOK BSP_GPIOK

//GPIO pin type define
#define SszPin BSP_PIN
//GPIO pin mask define
#define SSZ_PIN0                 ((unsigned short)0x0001)  /*!< Pin 0 selected */
#define SSZ_PIN1                 ((unsigned short)0x0002)  /*!< Pin 1 selected */
#define SSZ_PIN2                 ((unsigned short)0x0004)  /*!< Pin 2 selected */
#define SSZ_PIN3                 ((unsigned short)0x0008)  /*!< Pin 3 selected */
#define SSZ_PIN4                 ((unsigned short)0x0010)  /*!< Pin 4 selected */
#define SSZ_PIN5                 ((unsigned short)0x0020)  /*!< Pin 5 selected */
#define SSZ_PIN6                 ((unsigned short)0x0040)  /*!< Pin 6 selected */
#define SSZ_PIN7                 ((unsigned short)0x0080)  /*!< Pin 7 selected */
#define SSZ_PIN8                 ((unsigned short)0x0100)  /*!< Pin 8 selected */
#define SSZ_PIN9                 ((unsigned short)0x0200)  /*!< Pin 9 selected */
#define SSZ_PIN10                ((unsigned short)0x0400)  /*!< Pin 10 selected */
#define SSZ_PIN11                ((unsigned short)0x0800)  /*!< Pin 11 selected */
#define SSZ_PIN12                ((unsigned short)0x1000)  /*!< Pin 12 selected */
#define SSZ_PIN13                ((unsigned short)0x2000)  /*!< Pin 13 selected */
#define SSZ_PIN14                ((unsigned short)0x4000)  /*!< Pin 14 selected */
#define SSZ_PIN15                ((unsigned short)0x8000)  /*!< Pin 15 selected */
#define SSZ_PINAll               ((unsigned short)0xFFFF)  /*!< All pins selected */

//GPIO mode
typedef enum
{
	kSszGpioInputFloating, //Input and no Pull Mode
	kSszGpioInputPD, //Input and Pull Down Mode
	kSszGpioInputPU, //Input and Pull Up Mode
	kSszGpioOutOD, //Output and Open Drain Mode
	kSszGpioOutOD_PD, //Output and Open Drain Mode and Pull Down Mode
	kSszGpioOutOD_PU, //Output and Open Drain Mode and Pull Up Mode
	kSszGpioOutPP, //Output and Push Pull Mode
	kSszGpioOutPP_PD, //Output and Push Pull Mode and Pull Down Mode
	kSszGpioOutPP_PU, //Output and Push Pull Mode and Pull Up Mode
}SszGpioMode;


typedef struct {
	SszGpio gpio;
	SszPin pin_mask;
	int8_t valid_value;
}SszGpioPinEx;

typedef struct {
	SszGpio gpio;
	SszPin pin_mask;
}SszGpioPin;

#ifdef __cplusplus
extern "C" {
#endif
	
//set pin high
#define ssz_gpio_set(gpio, pin_mask) bsp_gpio_set(gpio, pin_mask)
#define ssz_gpio_quick_set(gpio, pin_mask) bsp_gpio_quick_set(gpio, pin_mask)
//set pin low
#define ssz_gpio_clear(gpio, pin_mask) bsp_gpio_clear(gpio, pin_mask)
#define ssz_gpio_quick_clear(gpio, pin_mask) bsp_gpio_quick_clear(gpio, pin_mask)
//get pin value, 1:high, 0:low
#define ssz_gpio_is_high(gpio, pin_mask) (bsp_gpio_get(gpio, pin_mask)!=0)
#define ssz_gpio_quick_is_high(gpio, pin_mask) (bsp_gpio_quick_get(gpio, pin_mask)!=0)

//below interface is simpler to set/clear/get pin,
//but first you need define the gpio_pin(have port and pin mask)
//set pin high
#define ssz_gpio_set_pin(gpio_pin) bsp_gpio_set(gpio_pin)
#define ssz_gpio_quick_set_pin(gpio_pin) bsp_gpio_quick_set(gpio_pin)
//set pin low
#define ssz_gpio_clear_pin(gpio_pin) bsp_gpio_clear(gpio_pin)
#define ssz_gpio_quick_clear_pin(gpio_pin) bsp_gpio_quick_clear(gpio_pin)
//get pin value, 1:high, 0:low
#define ssz_gpio_is_pin_high(gpio_pin) (bsp_gpio_get(gpio_pin)!=0)
#define ssz_gpio_quick_is_pin_high(gpio_pin) (bsp_gpio_quick_get(gpio_pin)!=0)

//set gpio direction
void ssz_gpio_set_mode(SszGpio gpio, SszPin pin_mask, SszGpioMode mode);
//enable clock
void ssz_gpio_enable_clock(SszGpio gpio);
bool ssz_gpio_is_valid(const SszGpioPinEx *gpio_pin);

#ifdef __cplusplus
}
#endif
