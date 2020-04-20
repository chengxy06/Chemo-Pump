/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-17 dczhang
* Initial revision.
*
************************************************/
#include "led.h"
#include "ssz_gpio.h"
#include "ssz_common.h"
#include "timer.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//static bool g_led_is_on;
static LedID g_led_show_id;
static int g_led_turn_on_time_ms;

static LedID g_led_last_show_id;
/************************************************
* Function 
************************************************/
void led_green_on()
{
    ssz_gpio_set(LED_GREEN_PORT,LED_GREEN_PIN);
}

void led_yellow_on()
{
    ssz_gpio_set(LED_YELLOW_PORT,LED_YELLOW_PIN);
}

void led_red_on()
{
    ssz_gpio_set(LED_RED_PORT,LED_RED_PIN);
}

void led_green_off()
{
    ssz_gpio_clear(LED_GREEN_PORT,LED_GREEN_PIN);
}

void led_yellow_off()
{
    ssz_gpio_clear(LED_YELLOW_PORT,LED_YELLOW_PIN);
}

void led_red_off()
{
    ssz_gpio_clear(LED_RED_PORT,LED_RED_PIN);
}

void led_init()
{
    led_green_off();
    led_yellow_off();
    led_red_off();
}

void led_turn_on_internal(LedID led_id)
{
	//ssz_traceln("turn on");
	assert(led_id < kLedMax);
	switch (led_id) {
	case kLedGreen:
		led_green_on();
		led_yellow_off();
		led_red_off();
		break;

	case kLedYellow:
		led_green_off();
		led_yellow_on();
		led_red_off();
		break;

	case kLedRed:
		led_green_off();
		led_yellow_off();
		led_red_on();
		break;

	default:
		ssz_assert_fail();
		break;
	}

	g_led_last_show_id = led_id;

}

void led_turn_off_internal()
{
	//ssz_traceln("turn off");
	led_green_off();
	led_yellow_off();
	led_red_off();
	g_led_last_show_id = kLedMax;
}

void led_turn_on(LedID led_id) {
	led_stop_flash();
	led_turn_on_internal(led_id);
}
void led_turn_off() {
	led_stop_flash();
	led_turn_off_internal();
}
LedID led_current_turn_on(){
	return g_led_last_show_id;
}

static void led_flash_period_timer_out() {
	led_turn_on_internal(g_led_show_id);
	timer_start_oneshot_after(kTimerLEDFlashOff, g_led_turn_on_time_ms);
}

void led_flash(LedID led_id, int flash_period_ms, int turn_on_time_ms)
{
	led_turn_on_internal(led_id);
	//g_led_is_on = true;
	g_led_show_id = led_id;
	g_led_turn_on_time_ms = turn_on_time_ms;
	low_power_timer_set_handler(kTimerLEDFlashPeriod, led_flash_period_timer_out);
	low_power_timer_set_handler(kTimerLEDFlashOff, led_turn_off_internal);
	if (turn_on_time_ms < flash_period_ms) {
		timer_start_periodic_every(kTimerLEDFlashPeriod, flash_period_ms);
		timer_start_oneshot_after(kTimerLEDFlashOff, turn_on_time_ms);
	}
}

void led_stop_flash()
{
	timer_stop(kTimerLEDFlashPeriod);
	timer_stop(kTimerLEDFlashOff);
}

//resume last state
void led_resume(){
	if(g_led_last_show_id!=kLedMax)
	{
		led_turn_on_internal(g_led_last_show_id);
	}
}

