#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
#define MSG_QUEUE_MAX_NUM 30

typedef enum {
	kMsgNull,
	kMsgTimeChanged,
	kMsgAlarmSet,
	kMsgAlarmClear,
	kMsgKeyStuck,
	kMsgBatteryLevelChange, //param: battery level
	kMsgExternPowerStatusChange, //param: is use AC
	//kMsgBatteryStatusChange,//param: is battery exist
	kMsgInstallStateChanged,//param: is install
	kMsgBatteryStatusWhenMotorRun, //param: battery level

    kMSgInfusionMotorRun,
	kMSgInfusionMotorStop,
    kMSgThreeValveMotorRun,
	kMSgThreeValveMotorStop,

    //kMsgOnInfusionPoint,
    kMsgInfusionStart, //param1: InfusionType, param2: refer DELIVERY_START_TO_RUN...
    kMsgInfusionPause,//param1: InfusionType,param2: refer DELIVERY_PAUSE_BY_OTHER...
	kMsgInfusionResume,//param1: InfusionType,param2: 
    kMsgInfusionStop,//param1: InfusionType,param2:refer DELIVERY_STOP_FROM_RUN...
	//kMsgInfusionChangeToBagBMeds,

	kMsgInfuseOnePointStart,//param: InfusionType, 
	kMsgInfuseOnePointStop,//param: InfusionType, 
	
	kMsgInfuseNextOnePointAtOnce, //infuse one point

	kMsgCloseDisplay,
	kMsgGoToSleep,
	kMsgInfusionNextToTestLifeTime,
	kMsgAlarmVoiceMute,
	kMsgAlarmVoiceUnMute,
	kMsgAlarmPauseTimeOut, //alarm pause time out, replay
	

	kMSgBeforeInfusionMotorRun,//the msg will handle at once

    kMsgIDMax
}MsgID;
	
