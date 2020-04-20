#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


// alarm id 
typedef enum
{
	kAlarmNull=-1,

	// remind priority alarm id
	kRemindAlarmStart=0,              //报警ID开始位
	kAlarmSTFSwitchSuccessID,         //三通阀切换成功
	//kAlarmInfusionAlmostOverID,     //输液即将结束
	kAlarmDayTotalOutOfRangeID,       //极限量报警
    kAlarmInfusionNearEmptyID,        //输液快结束
    kAlarmAdapterDisconnectedID,      //Adapter disconnected
	kRemindAlarmEnd,                  //remind 优先级报警ID结束

    //lowest priority alarm id
	kLowestAlarmStart=9,
    kAlarmLowBatteryID,             //低电量报警
    kLowestAlarmEnd,                //lowest 优先级报警ID结束
    
	//low priority alarm id    
	kLowAlarmStart=19,
	kAlarmBatteryNotExist,     //未装电池
	kAlarmInfusionPauseTooLongID,     //输液暂停时间过长
	kLowAlarmEnd,
	
	//high priority alarm id
	kHighAlarmstart=29,
	kAlarmOcclusionID,              //阻塞
	kAlarmInstallNotInPlace,        //未装夹到位
	kAlarmBatteryExhaustionID,       //电池耗尽
	kAlarmBubbleOrNoDoseID,         //气泡或无液报警
	kAlarmInfusionOverID,           //输液结束
	kHighAlarmEnd,

	//highest priority alarm id, almost are system error
	kHighestAlarmStart=39,
	kAlarmMasterMCUErrorID,           //主MCU故障
	kAlarmSlaverMCUErrorID,           //辅MCU故障
	kAlarmInfuMotorStuckID,           //输注电机故障
	kAlarmInfuMotorCurrentTooLargeID,
	kAlarmInfuMotorDirectionErrorID,  //输注电机故障 5
	kAlarmThreeValveMotorStuckID,     //三通阀电机位置传感器故障
	kAlarmThreeValveMotorCurrentTooLargeID,
	kAlarmThreeValveMotorDirectionErrorID,
	kAlarmThreeValveMotorNotFindPos,  //三通阀电机没在最大时间内找到位置传感器
	kAlarmOpticalCouplerErrorID,      //光耦没有信号 10
	kAlarmPressureSensorErrorID,      //压力传感器故障
	kAlarmBubbleSensorErrorID,        //气泡传感器故障
	kAlarmDataAccessErrorID,          //数据存储故障
	kAlarmUserParamReadErrorID,       //数据存储故障
	kAlarmKeyStuckID,                 //按键粘连     15
	kAlarmInfuMotorErrorBySlaverID,   //输注电机故障
    kAlarmRTCErrorID,                 //RTC故障

    kHighestAlarmEnd,
}AlarmID;
#ifdef __cplusplus
}
#endif


