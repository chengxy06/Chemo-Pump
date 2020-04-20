#include "record_log.h"
#include "ssz_record.h"
#include "ssz_common.h"
#include "ssz_time.h"
#include "data.h"
#include "config.h"

#if RECORD_LOG_MODULE_ENABLE
/************************************************
* Declaration
************************************************/
//there is max OPERATION_LOG_HEAD_NVRAM_SIZE/16 heads
#define OPERATION_LOG_HEAD_NVRAM_START_ADDRESS ((64+60)*1024)
//if the nvram's (erase)lifetime is 100,000, 
//the 16K(4096*4,can write 4096*4/16=1024 records) size will support over 100 million lifetime
//	detail: (4096*4)/16.0*100000erase=102,400,000erase
//and it support write one record each 4 second for 10 years.
//	detail: 102400000/10.0/356/24/60=19.97rec/min, 60/19.97=3.004sec/rec
#define OPERATION_LOG_HEAD_NVRAM_SIZE	(FLASH_ERASE_UNIT_SIZE*4)
//start after head
#define OPERATION_LOG_DATA_NVRAM_START_ADDRESS (OPERATION_LOG_HEAD_NVRAM_START_ADDRESS+OPERATION_LOG_HEAD_NVRAM_SIZE)
//if the nvram's (erase)lifetime is 100,000, 
//the 1000 records will support over 100 million lifetime
//	detail: 1000*100000erase=100,000,000erase
//and it support write one record each 4 second for 10 years.
//	detail: 100000000/10.0/356/24/60=19.50rec/min, 60/19.50=3.07/rec
//the nvram size is OPERATION_LOG_MAX_SIZE_AT_LEAST/(FLASH_ERASE_UNIT_SIZE/ONE_RECORD_LOG_SIZE)*FLASH_ERASE_UNIT_SIZE
//because we need checksum, so add SSZ_NVRAM_CHECKSUM_SIZE,
//because it will have decimal when div for erase units, so need add 1 erase unit
//because we must erase one unit when write new record, so need add 1 more erase unit(so no record will lost) 
// detail: (1000/(4096/(36+4))+2)*4096/1024=44KB
#ifdef OPERATION_LOG_MAX_SIZE
#define OPERATION_LOG_DATA_NVRAM_SIZE	\
	(OPERATION_LOG_MAX_SIZE/(FLASH_ERASE_UNIT_SIZE/(ONE_RECORD_LOG_SIZE+SSZ_NVRAM_CHECKSUM_SIZE))+2)*FLASH_ERASE_UNIT_SIZE
#else
#define OPERATION_LOG_DATA_NVRAM_SIZE	\
	(OPERATION_LOG_MAX_SIZE_AT_LEAST/(FLASH_ERASE_UNIT_SIZE/(ONE_RECORD_LOG_SIZE+SSZ_NVRAM_CHECKSUM_SIZE))+2)*FLASH_ERASE_UNIT_SIZE
#endif

#define HISTORY_LOG_HEAD_NVRAM_START_ADDRESS ((64)*1024)//((512+60)*1024)
//if the nvram's (erase)lifetime is 100,000, 
//the 16K(4096*4,can write 4096*4/16=1024 records) size will support over 100 million lifetime
//	detail: (4096*4)/16*100000erase=102,400,000erase
//and it support write one record each 4 second for 10 years.
//	detail: 102400000/10.0/356/24/60=19.97rec/min, 60/19.97=3.004sec/rec
#define HISTORY_LOG_HEAD_NVRAM_SIZE	(FLASH_ERASE_UNIT_SIZE*4)
//start after head
#define HISTORY_LOG_DATA_NVRAM_START_ADDRESS (HISTORY_LOG_HEAD_NVRAM_START_ADDRESS+HISTORY_LOG_HEAD_NVRAM_SIZE)
//if the nvram's (erase)lifetime is 100,000, 
//the 1000 records will support over 100 million lifetime
//	detail: 1000*100000erase=100,000,000erase
//and it support write one record each 4 second for 10 years.
//	detail: 100000000/10.0/356/24/60=19.50rec/min, 60/19.50=3.07/rec
//the nvram size is HISTORY_LOG_MAX_SIZE_AT_LEAST/(FLASH_ERASE_UNIT_SIZE/ONE_RECORD_LOG_SIZE)*FLASH_ERASE_UNIT_SIZE
//because we need checksum, so add SSZ_NVRAM_CHECKSUM_SIZE,
//because it will have decimal when div for erase units, so need add 1 erase unit
//because we must erase one unit when write new record, so need add 1 more erase unit(so no record will lost) 
// detail: (1000/(4096/(36+4))+2)*4096/1024=44KB
#ifdef HISTORY_LOG_MAX_SIZE
#define HISTORY_LOG_DATA_NVRAM_SIZE	\
	(HISTORY_LOG_MAX_SIZE/(FLASH_ERASE_UNIT_SIZE/(ONE_RECORD_LOG_SIZE+SSZ_NVRAM_CHECKSUM_SIZE))+2)*FLASH_ERASE_UNIT_SIZE
#else
#define HISTORY_LOG_DATA_NVRAM_SIZE	\
	(HISTORY_LOG_MAX_SIZE_AT_LEAST/(FLASH_ERASE_UNIT_SIZE/(ONE_RECORD_LOG_SIZE+SSZ_NVRAM_CHECKSUM_SIZE))+2)*FLASH_ERASE_UNIT_SIZE
#endif

/************************************************
* Variable 
************************************************/

static SszRecord g_logs[kLogTypeMax];
const static int g_logs_max_size[kLogTypeMax] = {
#ifdef OPERATION_LOG_MAX_SIZE
	OPERATION_LOG_MAX_SIZE,
#else
	-1,
#endif

#ifdef HISTORY_LOG_MAX_SIZE
	HISTORY_LOG_MAX_SIZE,
#else
	-1,
#endif
};
static bool g_log_is_inited = false;
/************************************************
* Function 
************************************************/


void record_log_init() {
	ssz_traceln("record log init begin");
	
	//OperationLog init
	ssz_record_struct_init(&g_logs[kOperationLog], data_nvram(), OPERATION_LOG_DATA_NVRAM_START_ADDRESS, 
		OPERATION_LOG_DATA_NVRAM_SIZE, ONE_RECORD_LOG_SIZE, true, true,
		OPERATION_LOG_HEAD_NVRAM_START_ADDRESS, OPERATION_LOG_HEAD_NVRAM_SIZE);
	ssz_assert(g_logs[kOperationLog].record_user_max_count >= g_logs_max_size[kOperationLog]);
	if (ssz_record_load(&g_logs[kOperationLog]) == kSszRecordLastHeadBroken) {
		ssz_trace("operation log last head broken\n");
	}

	//history init
	ssz_record_struct_init(&g_logs[kHistoryLog], data_nvram(), HISTORY_LOG_DATA_NVRAM_START_ADDRESS,
		HISTORY_LOG_DATA_NVRAM_SIZE, ONE_RECORD_LOG_SIZE, true, true,
		HISTORY_LOG_HEAD_NVRAM_START_ADDRESS, HISTORY_LOG_HEAD_NVRAM_SIZE);
	ssz_assert(g_logs[kHistoryLog].record_user_max_count >= g_logs_max_size[kHistoryLog]);
	if (ssz_record_load(&g_logs[kHistoryLog]) == kSszRecordLastHeadBroken) {
		ssz_trace("history log last head broken\n");
	}

	ssz_traceln("record log init end");
	g_log_is_inited = true;
}
bool record_log_is_inited(){
	return g_log_is_inited;
}

result_t record_log_add_info_with_data(LogType log_type, LogEvent log_event, const void* event_data, int event_size) {
	return record_log_add_with_data_ex(log_type, ssz_time_now_seconds(), 
		LOG_EVENT_TYPE_INFO, log_event, event_data, event_size);
}

result_t record_log_add_info(LogType log_type, LogEvent log_event, const LogVar* vars, int var_size) {
	uint8_t event_data[RECORD_LOG_EVENT_DATA_MAX_SIZE] = { 0 };
	int event_size = 0;

	if (vars && var_size > 0) {
		event_size = record_log_var_to_data(vars, var_size, event_data);
	}
	return record_log_add_with_data_ex(log_type, ssz_time_now_seconds(),
		LOG_EVENT_TYPE_INFO, log_event, event_data, event_size);
}

result_t record_log_add_with_data(LogType log_type, uint32_t event_type, LogEvent log_event, const void* event_data, int event_size) {
	return record_log_add_with_data_ex(log_type, ssz_time_now_seconds(),
		event_type, log_event, event_data, event_size);
}
result_t record_log_add(LogType log_type, uint32_t event_type,LogEvent log_event, const LogVar* vars, int var_size) {
	uint8_t event_data[RECORD_LOG_EVENT_DATA_MAX_SIZE] = { 0 };
	int event_size = 0;

	if (vars && var_size > 0) {
		event_size = record_log_var_to_data(vars, var_size, event_data);
	}
	return record_log_add_with_data_ex(log_type, ssz_time_now_seconds(),
		event_type, log_event, event_data, event_size);
}
result_t record_log_add_with_ints(LogType log_type, uint32_t event_type, LogEvent log_event, int var[], int var_size) {
	LogVar log_vars[4];
	ssz_check(var_size <= ssz_array_size(log_vars));
	for (int i = 0; i < var_size; i++) {
		log_vars[i].type = kLogVarInteger;
		log_vars[i].value = var[i];
	}
	return record_log_add(log_type, event_type, log_event, log_vars, var_size);
}
result_t record_log_add_with_one_int(LogType log_type, uint32_t event_type, LogEvent log_event, int var) {
	return record_log_add_with_ints(log_type, event_type, log_event, &var, 1);
}
result_t record_log_add_with_two_int(LogType log_type, uint32_t event_type, LogEvent log_event, int var1, int var2) {
	int vars[2];
	vars[0] = var1;
	vars[1] = var2;
	return record_log_add_with_ints(log_type, event_type, log_event, vars, ssz_array_size(vars));
}
result_t record_log_add_with_three_int(LogType log_type, uint32_t event_type, LogEvent log_event, int var1, int var2, int var3) {
	int vars[3];
	vars[0] = var1;
	vars[1] = var2;
	vars[2] = var3;
	return record_log_add_with_ints(log_type, event_type, log_event, vars, ssz_array_size(vars));
}
result_t record_log_add_with_four_int(LogType log_type, uint32_t event_type, LogEvent log_event, int var1, int var2, int var3, int var4) {
	int vars[4];
	vars[0] = var1;
	vars[1] = var2;
	vars[2] = var3;
	vars[3] = var4;
	return record_log_add_with_ints(log_type, event_type, log_event, vars, ssz_array_size(vars));
}
//add one record
//errors:  kSuccess,kError
result_t record_log_add_with_data_ex(LogType log_type, int32_t occur_time, uint32_t event_type, LogEvent log_event, const void* event_data, int event_size){
	LogOneRecord one_rc;
	result_t ret;

	ssz_assert(event_size <= RECORD_LOG_EVENT_DATA_MAX_SIZE);
	one_rc.sequence_number = g_logs[log_type].curr_sequence_number + 1;
	one_rc.occur_time = occur_time;
	one_rc.event_type = event_type;
	one_rc.event_id = log_event;
	ssz_mem_zero(one_rc.event_data, sizeof(one_rc.event_data));
	if (event_data && event_size>0) {
		memcpy(one_rc.event_data, event_data, event_size);
	}
	ret = ssz_record_push(&g_logs[log_type], &one_rc);
	if (ret != kSuccess) {
		return ret;
	}

	if(log_type != kOperationLog){
		one_rc.sequence_number = g_logs[kOperationLog].curr_sequence_number + 1;
		ret = ssz_record_push(&g_logs[kOperationLog], &one_rc);
	}

	return ret;
}
result_t record_log_add_ex(LogType log_type, int32_t occur_time, uint32_t event_type, LogEvent log_event, const LogVar* vars, int var_size){
	uint8_t event_data[RECORD_LOG_EVENT_DATA_MAX_SIZE]={0};
	int data_size = 0;

	if (vars && var_size > 0) {
		data_size = record_log_var_to_data(vars, var_size, event_data);
	}
	return record_log_add_with_data_ex(log_type, occur_time, event_type, log_event, event_data, data_size);
}

int record_log_get_base_index(LogType log_type) {
	int base = 0;
	if (g_logs_max_size[log_type]>0 && 
		ssz_record_size(&g_logs[log_type]) > g_logs_max_size[log_type]) {
		base = ssz_record_size(&g_logs[log_type]) - g_logs_max_size[log_type];
	}

	return base;
}

//get the record at the index, index range is 0~size-1
//return:
//	maybe kSuccess,kChecksumWrong,kInvalidParam,kError
result_t record_log_get(LogType log_type, int32_t index, LogOneRecord* one_record) {
	return ssz_record_at(&g_logs[log_type],
		record_log_get_base_index(log_type)+index, one_record);
}
result_t record_log_get_with_var(LogType log_type, int32_t index, LogOneRecord* one_record, LogVar* vars, int var_max_size) {
	result_t ret = ssz_record_at(&g_logs[log_type],
		record_log_get_base_index(log_type) +index, one_record);
	if (ret != kSuccess) {
		return ret;
	}
	for (int i = 0; i < var_max_size; i++) {
		vars[i].type = kLogVarNull;
	}
	record_log_data_to_var(one_record->event_data, vars, var_max_size);

	return ret;
}
result_t record_log_get_from_last(LogType log_type, int32_t index, LogOneRecord* one_record) {
	return ssz_record_at(&g_logs[log_type], ssz_record_size(&g_logs[log_type])-1-index, one_record);
}
result_t record_log_get_from_last_with_var(LogType log_type, int32_t index, LogOneRecord* one_record, LogVar* vars, int var_max_size) {
	result_t ret = ssz_record_at(&g_logs[log_type], ssz_record_size(&g_logs[log_type])-1-index, one_record);
	if (ret != kSuccess) {
		return ret;
	}
	for (int i = 0; i < var_max_size; i++) {
		vars[i].type = kLogVarNull;
	}
	record_log_data_to_var(one_record->event_data, vars, var_max_size);

	return ret;
}

//return the vars num 
int record_log_data_to_var(const void* event_data, LogVar* vars, int var_max_size) {
	const uint8_t* p = (const uint8_t*)event_data;
	const uint8_t* p_end = (uint8_t*)event_data+ RECORD_LOG_EVENT_DATA_MAX_SIZE;
	int var_size =0;
	while (p<p_end && *p != kLogVarNull && var_size<var_max_size) {
		vars[var_size].type = (LogVarType)*p;
		p++;
		if (vars[var_size].type == kLogVarString) {
			vars[var_size].str_value = (const char*)p;
			p += strlen((const char*)p) + 2;
		}
		else if (vars[var_size].type != kLogVarNull) {
			ssz_assert(p_end-p>=sizeof(vars[0].value));
			memcpy(&vars[var_size].value, p, sizeof(vars[0].value));
			p += sizeof(vars[0].value);
		}
		else
		{
			ssz_assert_fail();
		}
		var_size++;
	}

	return var_size;
}
//return the data size, the data should like this: uint8_t event_data[RECORD_LOG_EVENT_DATA_MAX_SIZE];
int record_log_var_to_data(const LogVar* vars, int var_size, void* event_data) {
	uint8_t* p = (uint8_t*)event_data;
	//int data_size = 0;
	for (int i = 0; i < var_size; i++) {
		*p = vars[i].type;
		p++;
		if (vars[i].type == kLogVarString) {
			strcpy((char*)p, vars[i].str_value);
			p += strlen(vars[i].str_value) + 1;
		}
		else if (vars[i].type != kLogVarNull) {
			memcpy(p, &vars[i].value, sizeof(vars[i].value));
			p += sizeof(vars[i].value);
		}
		else if(vars[i].type == kLogVarNull){
			p--;
			break;
		}
	}

	ssz_assert(p -(uint8_t*)event_data<= RECORD_LOG_EVENT_DATA_MAX_SIZE);
	return p - (uint8_t*)event_data;
}

//judge is empty
bool record_log_is_empty(LogType log_type) {
	return ssz_record_is_empty(&g_logs[log_type]);
}

int record_log_size(LogType log_type) {
	int32_t log_size = ssz_record_size(&g_logs[log_type]);

	if (record_log_get_base_index(log_type)==0) {
		return log_size;
	}
	else {
		return g_logs_max_size[log_type];
	}
}

const SszRecord* record_log_info(LogType log_type) {
	return &g_logs[log_type];
}

//return:
//	maybe kSuccess,kError
result_t record_log_clear(LogType log_type) {
	return ssz_record_clear(&g_logs[log_type]);
}
#endif
