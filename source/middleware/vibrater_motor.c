/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-20 dczhang
* Initial revision.
*
************************************************/
#include "vibrater_motor.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//run = true; stop = false
bool g_vibrator_motor_is_running;
/************************************************
* Function 
************************************************/
void vibrator_motor_init()
{
    g_vibrator_motor_is_running = false;

    ssz_gpio_clear(VIBRATOR_EN_PORT,VIBRATOR_EN_PIN);
}

void vibrator_motor_start()
{
    g_vibrator_motor_is_running = true;

    ssz_gpio_set(VIBRATOR_EN_PORT,VIBRATOR_EN_PIN);
}

void vibrator_motor_stop()
{
    g_vibrator_motor_is_running = false;

    ssz_gpio_clear(VIBRATOR_EN_PORT,VIBRATOR_EN_PIN);
}

