#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

//the data size which can be save at one record
//we store max 4 int variable, before each variable need one byte variable type
// so size: (4+1)*4=20
#define RECORD_LOG_EVENT_DATA_MAX_SIZE 20 
// if record have checksum , the real one record size will add CRC32 checksum(4 byte) size
#define ONE_RECORD_LOG_SIZE (sizeof(LogOneRecord)) 

//the log max size will = xxx
//#define OPERATION_LOG_MAX_SIZE 1000
#define HISTORY_LOG_MAX_SIZE 1000

//or the log max size will >= xxx
#ifndef OPERATION_LOG_MAX_SIZE 
#define OPERATION_LOG_MAX_SIZE_AT_LEAST 1000
#endif
#ifndef HISTORY_LOG_MAX_SIZE 
#define HISTORY_LOG_MAX_SIZE_AT_LEAST 1000
#endif

/*Event Type*/
//#define LOG_EVENT_TYPE_DEBUG           		 0x00000001
#define LOG_EVENT_TYPE_INFO           		 0x00000001
#define LOG_EVENT_TYPE_WARN           		 0x00000002
#define LOG_EVENT_TYPE_ERROR           		 0x00000004
//#define LOG_EVENT_TYPE_FATAL           		 0x00000010
#define LOG_EVENT_TYPE_INFUSION              0x00000010
#define LOG_EVENT_TYPE_ALARM   				 0x00000020
#define LOG_EVENT_TYPE_ALL   				 0xFFFFFFFF




typedef enum {
	kOperationLog,
	kHistoryLog,
	kLogTypeMax
}LogType;

typedef enum {
	//history log
	kLogEventHistroyStart=0,
	kLogEventLost= kLogEventHistroyStart,  //the record is damage
	kLogEventSystemError,
	kLogEventPowerOn,
	kLogEventPowerOff,

	kLogEventAlarm, //param: alarm_name

	//参数1:输注速度,	 参数2:总量, 参数3 : 冲管药量
	kLogEventInfusionStart,
	//参数1:峰值开始时间, 参数2 : 峰值结束时间, 参数3 : 峰值速度
	kLogEventInfusionPeekSet,
	//参数1:已输入的药量	参数2:已输入的冲管药量
	kLogEventInfusionPause,
	kLogEventInfusionResume,
	//param: 输注速度
	kLogEventChangeInfusionSpeed,
	//param: 输注总量
	kLogEventChangeInfusionVolume,
	//param:冲管药量
	kLogEventChangeCleanTubeVolume,
	//参数1:峰值开始时间, 参数2 : 峰值结束时间, 参数3 : 峰值速度
	kLogEventChangeInfusionPeekSet,
	kLogEventCloseInfusionPeekSet,
	//参数1:输入药量	参数2:输入冲管药量
	kLogEventInfusionStop,
	
	//参数1:冲管药量
	kLogEventCleanTubeStart,
	//参数1:已输入的冲管药量
	kLogEventCleanTubePause,
	kLogEventCleanTubeResume,
	//参数1:输入冲管药量
	kLogEventCleanTubeStop,

	kLogEventEnterManualClearAir,
	kLogEventEnterAutoClearAir,
	kLogEventClearAirStart,
	//参数1:排空药量		参数2 : 排空冲管药量
	kLogEventClearAirStop,
	//切换到小药袋
	kLogEventChangeToSmallBag,
	//切换到大药袋
	kLogEventChangeToBigBag,

	kLogEventHistroyEnd,

	//factory log
	kLogEventOperationStart = 1000,
	//param1: encoder_count_of_one_ml, param2: run volume
	kLogEventPrecisionCalibrationDone= kLogEventOperationStart,
	//param1: ad_diff, param2: adc_when_start
	kLogEventPressureCalibrationDone,
	//param1: slope, param2: one drop adc, param3: step
	kLogEventNewPressureCalibrationDone,
	//param1: ad_diff,
	kLogEventBatteryCalibrationDone,
	//param1:hour, param2:minute
	kLogEventSetTime,
	//param1:month, param2:day
	kLogEventSetDate,
	//param1:year
	kLogEventSetYear,
	//params: pressure_adc, adc_when_install, occlusion count, current pressure
	kLogEventOcclusionPressure,
	//params: battery voltage
	kLogEventBatteryLow,
	//params: battery voltage
	kLogEventBatteryExhausted,
	//params: RCC_CSR_register
	kLogEventMachineRestartArmCSR,	
	kLogEventMasterMcuResetByslaver,
	//params:reset_count
	kLogEventSlaverNotActive,
	//params:check_time, last_encoder
	kLogEventMotorStuck,
	//params:ad, run encoder
	kLogEventMotorCurrentTooLarge,
	//params:run encoder
	kLogEventMotorDirectionError,
	//params:check_time, last_optical_encoder, encoder
	kLogEventOpticalCouplerError,
	//params:is_to_a, encoder
	kLogEventThreeValveMotorDetectPos,
	//params:check_time, last_encoder
	kLogEventThreeValveMotorStuck,
	//params:ad, run encoder
	kLogEventThreeValveMotorCurrentTooLarge,
	//params:run encoder
	kLogEventThreeValveMotorDirectionError,
	//params:run encoder
	kLogEventThreeValveMotorNotFindPos,
	//params:is too big, ad
	kLogEventPressureSensorError,
	//params: data id,error code
	kLogEventDataWriteError,
	//params: data id,error code
	kLogEventDataReadError,
	//params: key
	kLogEventKeyStuck,
	//params: cause, run encoder
	kLogEventMotorStopBySlaver,
	//reset system
	kLogEventSystemReset,
    //params: user set max clean tube dose
    kLogEventMaxCleanTubeDose,
    //params: user set peak mode status
    kLogEventSetPeakMode,
    //params: user set infusion mode status
    kLogEventSetInfusionMode,
    //params: factory set bubble alarm status
    kLogEventSetBubbleAlarm,
    //RTC error
    kLogEventRTCError,
    //factory set pressure_threshold
    kLogEventPressureThreshold,
    //factory set pressure_threshold
    kLogEventPrecisionFactor,
    //auto power off
    kLogEventAutoPowerOff,
    
	kLogEventOperationEnd,
}LogEvent;

//saved size at nvram:16+20+4=40
typedef struct  {
	/*the sequence number of this record, it will be auto increased when write.*/
	int32_t sequence_number;
	/*The occur time of the event. It is seconds since SSZ_BASE_YEAR.*/
	int32_t occur_time;
	/*The event type.
	Each bit represents a event type.*/
	uint32_t event_type;
	int32_t event_id;//refer LogEvent

	uint8_t event_data[RECORD_LOG_EVENT_DATA_MAX_SIZE];
}LogOneRecord;


/*variable's type for variable event data*/
//each variable data is 4 bytes, if not, it will describe 
typedef enum
{
    /*no variable*/
    kLogVarNull = 0,
    /*variable is an integer*/
    kLogVarInteger ,
    /*if it is float type, the value should multiply by 1000 and change to integer*/
    kLogVarFloat, 
	/*if it is float type, the value should multiply by 100 and change to integer*/
	kLogVarFloatWithTwoDecimal,
	/*if it is float type, the value should multiply by 10 and change to integer*/
	kLogVarFloatWithOneDecimal,
    /*string*/
    kLogVarString, //variable data: dynamic bytes, must include null-terminator
	
	kLogVarAlarmID,
}LogVarType;


/*Variable for event data*/
typedef struct
{
	LogVarType type;/*the type of data1, refer LogVarType*/
	union{
		int32_t value;
		const char* str_value;
	};
}LogVar;


#ifdef __cplusplus
}
#endif

