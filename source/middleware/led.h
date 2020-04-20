/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-17 dczhang
* Initial revision.
*
************************************************/
#pragma once


#ifdef __cplusplus
extern "C" {
#endif


#include "ssz_def.h"
#include "dev_def.h"


#define LED_GREEN_PORT  LED_G_GPIO_Port
#define LED_GREEN_PIN   LED_G_Pin
#define LED_YELLOW_PORT LED_Y_GPIO_Port
#define LED_YELLOW_PIN  LED_Y_Pin
#define LED_RED_PORT    LED_R_GPIO_Port
#define LED_RED_PIN     LED_R_Pin


typedef enum{
    kLedGreen,
    kLedYellow,
    kLedRed,
    
    kLedMax
}LedID;

/************************************************
* Declaration
************************************************/

void led_init();


void led_turn_on(LedID led_id);
void led_turn_off();
LedID led_current_turn_on();

void led_flash(LedID led_id, int flash_period_ms, int turn_on_time_ms);
void led_stop_flash();

//resume last state
void led_resume();

#ifdef __cplusplus
}
#endif



