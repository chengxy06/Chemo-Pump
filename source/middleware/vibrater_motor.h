/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-20 dczhang
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "ssz_gpio.h"
#include "dev_def.h"



#define VIBRATOR_EN_PORT M_VIBRATOR_EN_GPIO_Port
#define VIBRATOR_EN_PIN M_VIBRATOR_EN_Pin

#ifdef __cplusplus
extern "C" {
#endif

//run = true; stop = false
extern bool g_vibrator_motor_is_running;

#ifdef __cplusplus
}
#endif


/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void vibrator_motor_init();

void vibrator_motor_start();

void vibrator_motor_stop();



#ifdef __cplusplus
}
#endif


