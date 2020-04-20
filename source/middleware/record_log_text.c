/************************************************
* DESCRIPTION:
*
*
************************************************/
#include "record_log_text.h"
#include "ssz_common.h"
#include "param.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/


//convert var to str, return the str
const char* record_log_var_to_str(const LogVar* one_var, char* buff, int buff_max_size)
{
	switch (one_var->type) {
	case kLogVarNull:
		buff[0] = 0;
		break;
	case kLogVarInteger:
		snprintf(buff, buff_max_size, "%d", one_var->value);
		break;
	case kLogVarFloat:
		snprintf(buff, buff_max_size, "%d.%03d", one_var->value / 1000, one_var->value % 1000);
		break;
	case kLogVarFloatWithOneDecimal:
		snprintf(buff, buff_max_size, "%d.%d", one_var->value / 10, one_var->value % 10);
		break;
	case kLogVarFloatWithTwoDecimal:
		snprintf(buff, buff_max_size, "%d.%02d", one_var->value / 100, one_var->value % 100);
		break;
	case kLogVarString:
		snprintf(buff, buff_max_size, "%s", one_var->str_value);
		break;
	case kLogVarAlarmID:
	{
		AlarmLevel alarm_level = alarm_id_to_level((AlarmID)(one_var->value));
		if (alarm_level >= kAlarmLevelHighest) {
			snprintf(buff, buff_max_size, "!!!%s", alarm_id_to_const_str((AlarmID)(one_var->value)));
		} else if (alarm_level >= kAlarmLevelHigh) {
			snprintf(buff, buff_max_size, "!!!%s", alarm_id_to_const_str((AlarmID)(one_var->value)));
		} else if (alarm_level >= kAlarmLevelLowest) {
			snprintf(buff, buff_max_size, "!%s", alarm_id_to_const_str((AlarmID)(one_var->value)));
		} else {
			snprintf(buff, buff_max_size, "%s", alarm_id_to_const_str((AlarmID)(one_var->value)));
		}
		break;
	}
	default:
		buff[0] = 0;
		ssz_assert_fail();
		break;
	}

	return buff;
}

//convert record to str, return the str
const char* record_log_to_str(const LogOneRecord* one_record, char* buff, int buff_max_size) {
	char param_str[4][LOG_VAR_TEXT_MAX_SIZE] = { 0 };
	const char* format = log_event_id_to_const_str((LogEvent)one_record->event_id);
	LogVar log_var[4];
	int var_size;
	var_size = record_log_data_to_var(one_record->event_data, log_var, ssz_array_size(log_var));
	for (int i = 0; i < var_size; i++) {
		record_log_var_to_str(&log_var[i], param_str[i], ssz_array_size(param_str[0]));
	}
	snprintf(buff, buff_max_size, format, param_str[0], param_str[1], param_str[2], param_str[3]);
	return buff;
}