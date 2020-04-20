#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
	kEventNewTick,
	kEventReceiveNewMsg,
	kEventPCUartReceiveNewData,
	kEventBTUartReceiveNewData,
	kEventMCUUartReceiveNewData,
	kEventUIViewDirty,
	kEventDisplayDirty,
	kEventInfusionMotorFinishTarget,
	kEventThreeValveMotorFinishTarget,
	kEventInfusionMotorDirectionWrong,
	kEventThreeValveMotorDirectionWrong,
	kEventInfusionMotorGetSpeedFinished,
	kEventIDMax
}EventID;

#ifdef __cplusplus
}
#endif


