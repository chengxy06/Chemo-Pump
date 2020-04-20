/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-28 dczhang
* Initial revision.
*
************************************************/
#include "alarm_play.h"
#include "timer.h"
#include "common.h"
#include "led.h"
#include "app_sleep.h"
#include "mid_common.h"
#ifdef SSZ_TARGET_MACHINE
#include "spi.h"
#endif
#include "param.h"

/************************************************
* Declaration
************************************************/
#define BEEP_MODE_VIBRATOR_DELAY_MS 200
#define LOW_MODE_VIBRATOR_DELAY_MS  300
#define HIGH_MODE_VIBRATOR_DELAY_MS 500
#define VIBRATOR_PLAY_TIME_MS       200
#define ALARM_PLAY_INTERVAL_MS      20000

#define LED_FLASH_PERIOD_MS 500
#define LED_FLASH_ON_TIME_MS 125

#define USE_HARDWARE_VOICE_PLAY_PERIODIC 1
/************************************************
* Variable 
************************************************/

bool g_is_mute_mode = false;	//¾²ÒôÄ£Ê½×´Ì¬±êÊ¶


static AlarmPlayType g_alarm_last_play_type = kAlarmPlayTypeMax;
static int g_play_beep_counter;
static int g_play_beep_expect_times;

/************************************************
* Function 
************************************************/

//play beep voice with vibrator
void alarm_play_beep_with_vibrator()
{
    voice_stop_play_beep_channel();
    vibrator_motor_stop();

//    low_power_timer_set_handler(kTimerVoiceBeepStop, voice_stop_play_beep_channel);
    low_power_timer_set_handler(kTimerVibratorStart, vibrator_motor_start);
    low_power_timer_set_handler(kTimerVibratorStop, vibrator_motor_stop);

    if(g_is_mute_mode == false)
        voice_play_beep_once();
    timer_start_oneshot_after(kTimerVibratorStart, BEEP_MODE_VIBRATOR_DELAY_MS);
    timer_start_oneshot_after(kTimerVibratorStop, BEEP_MODE_VIBRATOR_DELAY_MS + VIBRATOR_PLAY_TIME_MS);
}

void alarm_play_beep_without_vibrator()
{
    //voice_stop_play_beep_channel();
    vibrator_motor_stop();

    if(g_is_mute_mode == false)
        voice_play_beep_once();
}

void alarm_play_low()
{
    vibrator_motor_stop();

//    low_power_timer_set_handler(kTimerVoiceBeepStop, voice_stop_play_beep_channel);
    low_power_timer_set_handler(kTimerVibratorStart, vibrator_motor_start);
    low_power_timer_set_handler(kTimerVibratorStop, vibrator_motor_stop);

#if !USE_HARDWARE_VOICE_PLAY_PERIODIC
	voice_stop_play_low_channel();
    if(g_is_mute_mode == false)
        voice_play_low_once();
#endif
    timer_start_oneshot_after(kTimerVibratorStart, BEEP_MODE_VIBRATOR_DELAY_MS);
    timer_start_oneshot_after(kTimerVibratorStop, BEEP_MODE_VIBRATOR_DELAY_MS + VIBRATOR_PLAY_TIME_MS);
}

void alarm_play_low_without_vibrator()
{
    vibrator_motor_stop();

#if !USE_HARDWARE_VOICE_PLAY_PERIODIC
	voice_stop_play_low_channel();
    if(g_is_mute_mode == false)
        voice_play_low_once();
#endif
}

void alarm_play_high()
{
    vibrator_motor_stop();

//    low_power_timer_set_handler(kTimerVoiceBeepStop, voice_stop_play_beep_channel);
    low_power_timer_set_handler(kTimerVibratorStart, vibrator_motor_start);
    low_power_timer_set_handler(kTimerVibratorStop, vibrator_motor_stop);

#if !USE_HARDWARE_VOICE_PLAY_PERIODIC
	voice_stop_play_high_channel();
    if(g_is_mute_mode == false)
        voice_play_high_once();
#endif
    timer_start_oneshot_after(kTimerVibratorStart, BEEP_MODE_VIBRATOR_DELAY_MS);
    timer_start_oneshot_after(kTimerVibratorStop, BEEP_MODE_VIBRATOR_DELAY_MS + VIBRATOR_PLAY_TIME_MS);
}

void alarm_play_high_without_vibrator()
{

    vibrator_motor_stop();

#if !USE_HARDWARE_VOICE_PLAY_PERIODIC
	voice_stop_play_high_channel();
    if(g_is_mute_mode == false)
        voice_play_high_once();    
#endif
}


void alarm_play_ex(AlarmPlayType alarm_type, bool is_vibrator)
{	
    ssz_assert(alarm_type < kAlarmPlayTypeMax);

	//stop first
	alarm_stop_play();
    switch(alarm_type){
        case kAlarmPlayBeepPeriodic:
			if (is_vibrator) {
				low_power_timer_set_handler(kTimerVoicePlayInterval, alarm_play_beep_with_vibrator);
				alarm_play_beep_with_vibrator();
			}
			else {
				low_power_timer_set_handler(kTimerVoicePlayInterval, alarm_play_beep_without_vibrator);
				alarm_play_beep_without_vibrator();
			}
			break;

		case kAlarmPlayLowAlarm:
			if (is_vibrator) {
				low_power_timer_set_handler(kTimerVoicePlayInterval, alarm_play_low);
				alarm_play_low();
			}
			else {
				low_power_timer_set_handler(kTimerVoicePlayInterval, alarm_play_low_without_vibrator);
				alarm_play_low_without_vibrator();
			}
#if USE_HARDWARE_VOICE_PLAY_PERIODIC
			voice_play_low_cycle();
#endif
			led_turn_on(kLedYellow);
			break;

		case kAlarmPlayHighAlarm:
			if (is_vibrator) {
				low_power_timer_set_handler(kTimerVoicePlayInterval, alarm_play_high);
				alarm_play_high();
			}
			else {
				low_power_timer_set_handler(kTimerVoicePlayInterval, alarm_play_high_without_vibrator);
				alarm_play_high_without_vibrator();
			}
#if USE_HARDWARE_VOICE_PLAY_PERIODIC
			voice_play_high_cycle();
#endif
			led_flash(kLedRed, LED_FLASH_PERIOD_MS, LED_FLASH_ON_TIME_MS);
			break;
    }

	g_alarm_last_play_type = alarm_type;
    //set the alarm periodic
    timer_start_periodic_every(kTimerVoicePlayInterval, ALARM_PLAY_INTERVAL_MS);
}

//play voice immediately
void alarm_play(AlarmPlayType alarm_type)
{
    alarm_play_ex(alarm_type, false);
}


void alarm_stop_play_internal(bool is_stop_led_flash)
{
    //voice_stop_play_beep_channel();
    voice_stop_play_low_channel();
    voice_stop_play_high_channel();
    vibrator_motor_stop();
	timer_stop(kTimerVoicePlayInterval);
	timer_stop(kTimerVibratorStart);
	timer_stop(kTimerVibratorStop);
	if(alarm_is_play_pause()){
		timer_stop(kTimerVoicePlayPause);
		msg_post_int(kMsgAlarmVoiceUnMute, 0);
	}
	if (is_stop_led_flash) {
		led_stop_flash();
		led_turn_off();
	}
	g_alarm_last_play_type = kAlarmPlayTypeMax;
}
void alarm_stop_play() {
	alarm_stop_play_internal(true);
}
void alarm_pause_timer_out() {
	if(	system_mode()==kSystemModeSleep){
#ifdef SSZ_TARGET_MACHINE
		ssz_gpio_enable_clock(PORTX_ISD2360_CS);
		ssz_gpio_set_mode(PORTX_ISD2360_CS, PINX_ISD2360_CS, kSszGpioOutPP);
		MX_SPI1_Init();
#endif
	}
	if (g_alarm_last_play_type!=kAlarmPlayTypeMax) {
		alarm_play(g_alarm_last_play_type);
		msg_post_int(kMsgAlarmPauseTimeOut, 0);
	}
	msg_post_int(kMsgAlarmVoiceUnMute, 0);
}
//it will pause for 2 minutes
void alarm_pause_play() {
	AlarmPlayType tmp =  g_alarm_last_play_type;
	timer_stop(kTimerVoicePlayPause);
	alarm_stop_play_internal(false);
	g_alarm_last_play_type = tmp;
	low_power_timer_set_handler(kTimerVoicePlayPause, alarm_pause_timer_out);
	timer_start_oneshot_after(kTimerVoicePlayPause, ALARM_PAUSE_TIME_MS);
	msg_post_int(kMsgAlarmVoiceMute, 0);
}
bool alarm_is_play_pause(){
	if(timer_is_actived(kTimerVoicePlayPause)){
		return true;
	}else{
		return false;
	}
}

void alarm_play_beep_with_count() 
{
    if(g_play_beep_counter<g_play_beep_expect_times){
		alarm_play_beep_without_vibrator();
        g_play_beep_counter ++;
    }else{
        timer_stop(kTimerVoicePlayBeepSeveralTimes);
    }
}

void alarm_play_beep_several_times(int count, int time_interval_ms) 
{
    g_play_beep_expect_times=count;
	g_play_beep_counter = 0;
    low_power_timer_set_handler(kTimerVoicePlayBeepSeveralTimes, alarm_play_beep_with_count);
	timer_start_periodic_every(kTimerVoicePlayBeepSeveralTimes, time_interval_ms);
}
void alarm_stop_play_beep_several_times(){
	timer_stop(kTimerVoicePlayBeepSeveralTimes);
}
bool alarm_is_play_beep_several_times(){
	if (timer_is_actived(kTimerVoicePlayBeepSeveralTimes)){
		return true;
	}else{
		return false;
	}

}


//is the voice and led can power off
bool alarm_play_is_power_can_off(){
	if (timer_is_actived(kTimerVoicePlayInterval)||
		timer_is_actived(kTimerVoicePlayBeepSeveralTimes)||
		timer_is_actived(kTimerVoicePlayPause)){
		return false;
	}else{
		return true;
	}
}

