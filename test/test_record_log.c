#ifdef TEST
#include "greatest.h"
#include "record_log.h"
#include <stdio.h>
#include <stdlib.h>
/***********************************************/
//this will call before every case at this suite
static void test_record_log_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_record_log_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_record_log_init() {
}

static void test_record_log_add() {
	for (int i = kLogEventHistroyStart; i < kLogEventHistroyEnd; i++) {
		record_log_add_with_four_int(kHistoryLog, LOG_EVENT_TYPE_INFO, i, 100, 200, 300, 400);
	}
	for (int i = kLogEventOperationStart; i < kLogEventOperationEnd; i++) {
		record_log_add_with_four_int(kOperationLog, LOG_EVENT_TYPE_INFO, i, 100, 200, 300, 400);
	}
}

static void test_record_log_add_with_data() {
}

static void test_record_log_get_from_last() {
}

static void test_record_log_get_from_last_with_var() {
}

static void test_record_log_data_to_var() {

}

static void test_record_log_var_to_data() {
	LogVar vars[10] = { { kLogVarInteger, 34 },{ kLogVarFloat, 35 },
	{ kLogVarInteger, 36 },
	{ kLogVarInteger, 37 },{ kLogVarString, 1 },{kLogVarNull,0}
	};
    vars[4].str_value = "hello";
	LogVar var2[10];

	uint8_t event_data[RECORD_LOG_EVENT_DATA_MAX_SIZE];

	GASSERT(record_log_var_to_data(vars, 4, event_data)==20);
	GASSERT(record_log_data_to_var(event_data, var2, 10) == 4);
	GASSERT(var2[0].type == kLogVarInteger && var2[0].value == 34);
	GASSERT(var2[1].type == kLogVarFloat&& var2[1].value == 35);
	GASSERT(record_log_data_to_var(event_data, var2, 2) == 2);

	GASSERT(record_log_var_to_data(vars+3, 4, event_data) == 12);
	GASSERT(record_log_data_to_var(event_data, var2, 10) == 2);
	GASSERT(var2[0].type == kLogVarInteger && var2[0].value == 37);
	GASSERT(var2[1].type == kLogVarString&& strcmp(var2[1].str_value,"hello") == 0);
}

static void test_record_log_is_empty() {
}

static void test_record_log_size() {
}

static void test_record_log_clear() {
}


/***********************************************/
#include "record_log_suite_def.h"
#endif


