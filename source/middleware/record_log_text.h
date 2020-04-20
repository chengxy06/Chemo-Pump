/************************************************
* DESCRIPTION:
*
* convert record log to text
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "record_log.h"
/************************************************
* Declaration
************************************************/

#define LOG_TEXT_MAX_SIZE 128
#define LOG_VAR_TEXT_MAX_SIZE 40

#ifdef __cplusplus
extern "C" {
#endif


//convert var to str, return the str
const char* record_log_var_to_str(const LogVar* one_var, char* buff, int buff_max_size);

//convert record to str, return the str
const char* record_log_to_str(const LogOneRecord* one_record, char* buff, int buff_max_size);


#ifdef __cplusplus
}
#endif

