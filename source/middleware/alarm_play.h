/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-28 dczhang
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "alarm.h"

/************************************************
* Declaration
************************************************/

typedef enum {
	kAlarmPlayBeepPeriodic,
	kAlarmPlayLowAlarm,
	kAlarmPlayHighAlarm,
	kAlarmPlayTypeMax
}AlarmPlayType;

#ifdef __cplusplus
extern "C" {
#endif


//play voice immediately
void alarm_play(AlarmPlayType alarm_type);
void alarm_play_ex(AlarmPlayType alarm_type, bool is_vibrator);

void alarm_stop_play();
//it will pause for 2 minutes
void alarm_pause_play();
bool alarm_is_play_pause();
void alarm_play_beep_several_times(int count, int time_interval_ms);
void alarm_stop_play_beep_several_times();
bool alarm_is_play_beep_several_times();
//is the voice and led can power off
bool alarm_play_is_power_can_off();

#ifdef __cplusplus
}
#endif






