#pragma once
#include "ssz_def.h"
#include "record_log_config.h"
#include "ssz_errorcode.h"
#include "ssz_record.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


void record_log_init();
bool record_log_is_inited();

//add one record
//errors:  kSuccess,kError
result_t record_log_add_info_with_data(LogType log_type, LogEvent log_event, const void* event_data, int event_size);
result_t record_log_add_info(LogType log_type, LogEvent log_event, const LogVar* vars, int var_size);

result_t record_log_add_with_data(LogType log_type, uint32_t event_type, LogEvent log_event, const void* event_data, int event_size);
result_t record_log_add(LogType log_type, uint32_t event_type, LogEvent log_event, const LogVar* vars, int var_size);
result_t record_log_add_with_one_int(LogType log_type, uint32_t event_type, LogEvent log_event, int var);
result_t record_log_add_with_two_int(LogType log_type, uint32_t event_type, LogEvent log_event, int var1, int var2);
result_t record_log_add_with_three_int(LogType log_type, uint32_t event_type, LogEvent log_event, int var1, int var2, int var3);
result_t record_log_add_with_four_int(LogType log_type, uint32_t event_type, LogEvent log_event, int var1, int var2, int var3, int var4);

result_t record_log_add_with_data_ex(LogType log_type, int32_t occur_time, uint32_t event_type, LogEvent log_event, const void* event_data, int event_size);
result_t record_log_add_ex(LogType log_type, int32_t occur_time, uint32_t event_type, LogEvent log_event, const LogVar* vars, int var_size);

//get the record at the index, index range is 0~size-1
//errors:
//	maybe kSuccess,kChecksumWrong,kInvalidParam,kError
result_t record_log_get(LogType log_type, int32_t index, LogOneRecord* one_record);
result_t record_log_get_with_var(LogType log_type, int32_t index, LogOneRecord* one_record, LogVar* vars, int var_max_size);
result_t record_log_get_from_last(LogType log_type, int32_t index, LogOneRecord* one_record);
result_t record_log_get_from_last_with_var(LogType log_type, int32_t index, LogOneRecord* one_record, LogVar* vars, int var_max_size);

//return the vars num 
int record_log_data_to_var(const void* event_data, LogVar* vars, int var_max_size);
//return the data size, the data should like this: uint8_t event_data[RECORD_LOG_EVENT_DATA_MAX_SIZE];
int record_log_var_to_data(const LogVar* vars, int var_max_size, void* event_data);

//judge is empty
bool record_log_is_empty(LogType log_type);

int record_log_size(LogType log_type);
const SszRecord* record_log_info(LogType log_type);

//errors: maybe kSuccess,kError
result_t record_log_clear(LogType log_type);

#ifdef __cplusplus
}
#endif


