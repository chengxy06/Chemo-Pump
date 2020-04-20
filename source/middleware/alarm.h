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
#include "ssz_list.h"
#include <stdio.h>
#include <stdlib.h>
#include "ssz_common.h"
#include "msg.h"
#include "timer.h"
#include "voice.h"
#include "vibrater_motor.h"
#include "msg.h"
#include "alarm_config.h"

#define REMIND_ALARM_MAX_SIZE (kRemindAlarmEnd-kRemindAlarmStart-1)
#define LOWEST_ALARM_MAX_SIZE (kLowestAlarmEnd- kRemindAlarmEnd - 1)
#define LOW_ALARM_MAX_SIZE (kLowAlarmEnd - kLowestAlarmEnd-1)
#define HIGH_ALARM_MAX_SIZE (kHighAlarmEnd - kLowAlarmEnd- 1)
#define HIGHEST_ALARM_MAX_SIZE (kHighestAlarmEnd - kHighAlarmEnd - 1)

typedef enum{
	kAlarmLevelNone,
	kAlarmLevelRemind,
	kAlarmLevelLowest,	
	kAlarmLevelLow,
	kAlarmLevelHigh,
	kAlarmLevelHighest,//used for system error
	
	kAlarmLevelMax 
}AlarmLevel;


/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern bool g_is_mute_mode;	//静音模式状态标识


void alarm_init();
AlarmLevel alarm_id_to_level(AlarmID alarm_id);
//报警ID存在，则返回true
//bool alarm_list_node_is_exist(AlarmID alarm_id, SszList* container);
//int alarm_node_pos(SszList* container, AlarmID alarm_id);
//保存报警ID并产生消息
void alarm_set(AlarmID alarm_id);
//清除报警信息并产生消息
void alarm_clear(AlarmID alarm_id);
bool alarm_is_set(AlarmID alarm_id);
AlarmLevel alarm_highest_priority_level();
//获取最高等级的最新报警ID
AlarmID alarm_highest_priority_id();


#ifdef __cplusplus
}
#endif






