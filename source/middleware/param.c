/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-11-10 xqzhao
* Initial revision.
*
************************************************/
#include "param.h"
#include "string_data.h"
#include "record_log.h"
/************************************************
* Declaration
************************************************/
typedef struct{
	int value;
	StrID str_id;
}StrIntItem;

/************************************************
* Variable 
************************************************/
BatteryLevel g_current_battery_level = kBatteryFour;
bool g_current_is_use_extern_power = false;
//bool g_current_battery_is_exist= true;
int g_encoder_count_of_one_ml = 1000;
bool g_is_pressure_adc_ready_when_installed=false;
int g_pressure_adc_when_installed=0;
bool g_is_pill_box_installed = false;
int g_battery_offset_ad = 0;
bool g_is_allow_bubble_alarm = true;
int g_pressure_threshold;

/***********************************************
* Function 
************************************************/



//dose unit: 0.001mL
int dose_to_encoder(int dose)
{
	return (dose*g_encoder_count_of_one_ml+999) / 1000;
}

//dose unit: 0.001mL
int encoder_to_dose(int encoder)
{
	return encoder * 1000 / g_encoder_count_of_one_ml;
}

float encoder_to_dose_float(int encoder)
{
	return encoder * 1000.0 / g_encoder_count_of_one_ml;
}

void set_encoder_count_of_one_ml(int encoder_count) {
 	g_encoder_count_of_one_ml = encoder_count;
}

void set_battery_offset_ad(int offset)
{
	g_battery_offset_ad = offset;
}

void set_pressure_threshold(int threshold)
{
	g_pressure_threshold = threshold;
}
int get_pressure_threshold(void)
{
	return g_pressure_threshold;
}


void set_bubble_alarm_status(bool status)
{
	g_is_allow_bubble_alarm = status;
}

bool is_bubble_alarm_enable(void)
{
	return g_is_allow_bubble_alarm;
}

//note: all str is get from string data, if need format, you need format it
const char* alarm_id_to_const_str(AlarmID alarm_id) {
	const char* ret = "";
	if (alarm_id_to_level(alarm_id) == kAlarmLevelHighest) {
		ret = get_string(kStrSystemError);
	}
	else {
		switch (alarm_id) {
		case kAlarmLowBatteryID:
			ret = get_string(kStrLowBattery);
			break;
		case kAlarmInfusionNearEmptyID:
			ret = get_string(kStrInfusionNearEmpty);
			break;
		case kAlarmBatteryNotExist:
			ret = get_string(kStrBatteryNotExist);
			break;
		case kAlarmInfusionPauseTooLongID:
			ret = get_string(kStrInfusionPauseToolLong);
			break;
		case kAlarmOcclusionID:
			ret = get_string(kStrTubeBlocked);
			break;
		case kAlarmInstallNotInPlace:
			ret = get_string(kStrInstallNotInPlace);
			break;
		case kAlarmBatteryExhaustionID:
			ret = get_string(kStrBatteryExhaustion);
			break;
		case kAlarmBubbleOrNoDoseID:
			ret = get_string(kStrHaveBubble);
			break;
		case kAlarmInfusionOverID:
			ret = get_string(kStrInfusionEnd);
			break;
       case kAlarmAdapterDisconnectedID:
			ret = get_string(kStrAdapterDisconnected);
			break;
		default:
			ssz_assert_fail();
			break;
		}
	}
	return ret;
}

//note: all str is get from string data, if need format, you need format it
const char* log_event_type_to_const_str(uint32_t log_event_type) {
	const StrIntItem items[] = {
		{ LOG_EVENT_TYPE_INFO, kStrLogTypeGeneral },
		{ LOG_EVENT_TYPE_ERROR, kStrLogTypeError },
		{ LOG_EVENT_TYPE_INFUSION, kStrLogTypeInfusion},
		{ LOG_EVENT_TYPE_ALARM, kStrLogTypeAlarm},
	};

	for (int i = 0; i < ssz_array_size(items); i++) {
		if (items[i].value == log_event_type) {
			return get_string(items[i].str_id);
		}
	}

	return "";
}

//note: all str is get from string data, if need format, you need format it
const char* log_event_id_to_const_str(LogEvent log_event_id) {
    const char* ret = "";

	if (log_event_id< kLogEventHistroyEnd) {
		return get_string((StrID)((int)kStrLogEventLost + log_event_id));
	}
	else {
        switch (log_event_id) {
            case kLogEventPrecisionCalibrationDone:
                ret="Precision calibration, \nencoder:%s, run volume:%suL";
                break;
            case kLogEventPressureCalibrationDone:
                ret="Pressure calibration, \ndiff:%s, start_adc:%s";
                break;
			case kLogEventNewPressureCalibrationDone:
				ret="Pressure calibration, \nslope:%s, one drop:%s step:%s";
				break;
			case kLogEventBatteryCalibrationDone:
				ret = "Battery calibration, \ndiff:%s";
				break;
            case kLogEventSetTime:
                ret="Set time to: \nhour %s, minute %s";
                break;
            case kLogEventSetDate:
                ret="Set date to: \nmonth %s, day %s";
                break;
            case kLogEventSetYear:
                ret="Set year to: %s";
                break;
            case kLogEventOcclusionPressure:
                ret="Meet Occlusion, \ncurr:%s=%skPa, start:%s, count:%s";
                break;
            case kLogEventBatteryLow:
                ret="Battery low voltage: %s";
                break;
            case kLogEventBatteryExhausted:
                ret="Battery exhausted voltage: %s";
                break;
            case kLogEventMachineRestartArmCSR:
                ret="RCC CSR for decimal: %s";
                break;	
            case kLogEventMasterMcuResetByslaver:
                ret="Master Mcu Reset By slaver";
                break;	
			case kLogEventSlaverNotActive:
				ret = "reset slaver %s";
				break;
			case kLogEventMotorStuck:
				ret = "motor stuck, check_time:%s\nlast_encoder:%s";
				break;
			case kLogEventMotorCurrentTooLarge:
				ret = "motor current large, \nad:%s encoder:%s";
				break;
			case kLogEventMotorDirectionError:
				ret = "motor direction error, \nencoder:%s";
				break;
			case kLogEventOpticalCouplerError:
				ret = "optical coupler error,\ncheck_time:%s last_encoder:%s encoder:%s";
				break;
			case kLogEventThreeValveMotorDetectPos:
				ret = "detect pos a:%s, encoder:%s";
				break;
			case kLogEventThreeValveMotorStuck:
				ret = "three motor stuck, check_time:%s\nlast_encoder:%s";
				break;
			case kLogEventThreeValveMotorCurrentTooLarge:
				ret = "three motor current large, \nad:%s encoder:%s";
				break;
			case kLogEventThreeValveMotorDirectionError:
				ret = "three motor direction error, \nencoder:%s";
				break;
			case kLogEventThreeValveMotorNotFindPos:
				ret = "three motor not find pos, \nencoder:%s";
				break;
			case kLogEventDataWriteError:
				ret = "data write error, \nid:%s, code:%s";
				break;
			case kLogEventDataReadError:
				ret = "param read error, \nid:%s, code:%s";
				break;
			case kLogEventKeyStuck:
				ret = "key stuck, \nkey:%s";
				break;
			case kLogEventMotorStopBySlaver:
				ret = "stop by slaver, \ncause:%s, encoder:%s";
				break;
			case kLogEventSystemReset:
				ret = "Reset system done";
				break;
			case kLogEventMaxCleanTubeDose:
				ret = "User set clean tube dose:%s";
				break;
			case kLogEventSetPeakMode:
				ret = "User set peak mode %s";
				break;
			case kLogEventSetInfusionMode:
				ret = "User set infution mode to %s";
				break;
			case kLogEventSetBubbleAlarm:
				ret = "factory set bubble alarm %s";
				break;
            case kLogEventPressureThreshold:
                ret = "factory set pressure threshold %s";
                break;
            case kLogEventPrecisionFactor:
                ret = "factory set precision factor %s";
                break;
            case kLogEventAutoPowerOff:
                ret = "auto power off";
                break;

            default:
                break;
        }
        return ret;
	}
}
