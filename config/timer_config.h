#pragma once
#include "ssz_def.h"
#include "config.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


//#define TIMER_QUEUE_MAX_NUM 20

#define TIMER_GROUP_NORMAL 0x01
// the timer id is relate with screen, So when exit screen, the id will auto stop
#define TIMER_GROUP_SCREEN 0x02 
#define TIMER_GROUP_LOW_POWER_MODE 0x04
#define TIMER_GROUP_ALL 0xFF

#define low_power_timer_set_handler(id, func) timer_set_handler_and_group(id, func, TIMER_GROUP_LOW_POWER_MODE)

typedef enum {
	//screen timer
	kTimerScreen, //used for screen,can be used at each screen
	kTimerScreen1, //used for screen,can be used at each screen
	kTimerScreen2, //used for screen,can be used at each screen
	kTimerScreen3, //used for screen,can be used at each screen
	kTimerScreen4, //used for screen,can be used at each screen

	//other timer
#if DTIMER_MODULE_ENABLE
	kTimerHandleDTimer,
#endif	
	kTimerScanKey,
	kTimerScanRTC,
	kTimerRepeatCmd,
	kTimerFlushToNvram,
	kTimerHandleUIViewTimer,
    //kTimerVoiceBeepStop,
    //kTimerVoiceLowStop,
    //kTimerVoiceHighStop,
    kTimerVibratorStart,
    kTimerVibratorStop,
	kTimerVoicePlayInterval,
	kTimerVoicePlayPause,
	kTimerVoicePlayBeepSeveralTimes,

	kTimerInfusionMotorCheck,
	kTimerInfusionPointCheck,
	kTimerThreeValveMotorCheck,
	kTimerStartInfusionAfterMs,
	kTimerInfusionMotorSoftRun,

	kTimerTestPlay,

	//monitor
	kTimerInfusionMonitor,
    kTimerSystemMonitor,
	kTimerInfusionMotorMonitor,
	kTimerWatchdog,
	kTimerWatchdogDelayStart,
	kTimerResetDelay,
    
	kTimerCommTimeTooLong,

	kTimerLEDFlashPeriod, //flash LED
	kTimerLEDFlashOff, //LED off
	kTimerNoOperateWhenPause, //set alarm to notify no operate when pause
	kTimerCheckEncoderAfterMotorStop,
	kTimerNoOperate, //no operate and need sleep
	kTimerTryToSleep, //try to sleep after close display
	kTimerHoldPowerOffKey,
	kTimerCloseRemind,
	kTimerAutoCloseTipPopup,
	kTimerAutoCloseUnlockTip,
	kTimerAutoLockKeypad,
	kTimerGetPressureAfterInstall,
	kTimerEnableSleep,
	kTimerInfusionNextToTestLifetime,
	kTimerWakeUpSlaverMcutime,	
	kTimerBatteryEmptyDelayToPoweroff,
	kTimerDelayBeforeBatteryLowAlarm,
		
	kTimerIDMax
}TimerID;

#ifdef __cplusplus
}
#endif


