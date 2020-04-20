#pragma once
#include "ssz_def.h"
#include "ssz_time.h"
/************************************************
* Declaration
************************************************/
typedef enum {
	kSszTimeFormat12Hour,
	kSszTimeFormat24Hour,	
}SszTimeFormat;

typedef enum {
	kSszDateFormat_DD_MM_YYYY,
    kSszDateFormat_MM_DD_YYYY,        
    kSszDateFormat_YYYY_MM_DD,
}SszDateFormat;

#ifdef __cplusplus
extern "C" {
#endif

//e.g. 2000 is leap year, 2001 is not leap year
bool ssz_is_leap_year(int year);

//return the day count at one month, 
//return: 30 or 31 or 29 or 28
int ssz_max_days_in_month(int year, int month);

//calc the week day of the day
//return 0~6 -> sunday, monday,...
int ssz_week_day(int year, int month, int day);

//the start time is SSZ_BASE_YEAR.1.1 00:00:00, like mktime
int32_t ssz_time_to_seconds(const SszDateTime *time);

//the start time is SSZ_BASE_YEAR.1.1 00:00:00, like mktime
int32_t ssz_time_to_seconds_ex(const SszDateTime *time, int time_zone);

//the start time is SSZ_BASE_YEAR.1.1 00:00:00 , like gmtime or localtime
void ssz_seconds_to_time(int32_t seconds, SszDateTime *time);

//the start time is SSZ_BASE_YEAR.1.1 00:00:00 , like gmtime or localtime
void ssz_seconds_to_time_ex(int32_t seconds, SszDateTime *time, int time_zone);

//it will find three number as year, month, day
//e.g. "2015-01-23"  ->2015 1 23
SszDate ssz_str_to_date(const char* date_str, SszDateFormat date_format);

//it will find three number as hour, min, second
//e.g. "17:42:49"  ->17 42 49
SszTime ssz_str_to_time(const char* time_str);

//convert to days, HH:MM:SS.xxx
//return rest milliseconds
int ssz_milliseconds_to_run_time_of_days(int32_t milliseconds, int *days, int *hour, int* minute, int *second);
//convert to hours, MM:SS.xxx
//return rest milliseconds
int ssz_milliseconds_to_run_time_of_hours(int32_t milliseconds, int *hours, int* minute, int *second);
#ifdef __cplusplus
}
#endif