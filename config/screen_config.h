#pragma once
#include "ssz_def.h"
#include "ssz_ui_view_config.h"
/************************************************
* Declaration
************************************************/

#define SCREEN_QUEUE_MAX_NUM 20

typedef enum {
	kScreenNull = kUI_IDUserStart,
	kScreenHome,
	kScreenMainMenu,
	kScreenHistoryLog,
	kScreenScrSetTotalDose,
	kScreenScrSetFisrtInfusionDose,
	kScreenScrSetLimitInfusionSpeed,
	kScreenScrTaskConfirm,
	kScreenScrSetPeekModeDuration,
	kScreenScrSetInfusionSpeed,
	kScreenScrSetInfusionDuration,
	kScreenScrSetPeekModeSpeed,
	kScreenPopup,
	kScreenSelectPopup,
	kScreenAlternativePopup,
	kScreenTipPopup,
	kScreenScrManualClearAir,
	kScreenScrAutoClearAir,
	kScreenScrInfusion,
	kScreenAlarm,
	kScreenCleanTube,
	kScreenScrSetTime,
	kScreenScrSetMenu,
	kScreenSetCleanTubeDose,
	kScreenScrSetDate,
	kScreenScrSetYear,
	kScreenSleep,
	kScreenPasswordPopup,
	kScreenKeypadLockTipPopup,
	kScreenKeypadUnlockTip,
	kScreenPressWrongToUnlockKeypad,
	kScreenScrSetMaxCleanTubeDose,

	//factory function
	kScreenFactoryMode,
	kScreenPressureCalibration,
	kScreenNewPressureCalibration,
	kScreenPrecisionCalibration,
	kScreenBatteryCalibration,
	kScreenScrChangePassword,
	kScreenScrDefaultValue,
	kScreenScrChangeOledBrightness,
	kScreenScrChangeVoiceVolume,
	kScreenOperationLog,
	kScreenPrecisionFactor,
	kScreenPressureThreshold,

    kScreenIDMax
}ScreenID;




	