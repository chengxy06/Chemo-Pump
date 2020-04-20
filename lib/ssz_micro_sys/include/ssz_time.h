#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
#define SSZ_SEC_IN_MIN  60
#define SSZ_SEC_IN_HOUR 3600 
#define SSZ_SEC_IN_DAY  86400

#define SSZ_TIME_STR_SIZE 20  //e.g. 2017-10-04 22:45:30
#define SSZ_TIME_H_STR_SIZE 10  //e.g. 22:45:30
typedef struct
{
	int16_t year;//range: SSZ_BASE_YEAR ~ 
	int8_t month;//range: 1 ~ 12
	int8_t day;//range: 1 ~ 31
	int8_t weekday;//range: 0~6=Sunday,Monday,...
}SszDate;

typedef struct
{
	int8_t hour;//range: 0 ~ 23
	int8_t minute;//range: 0 ~ 59
	int8_t second;//range: 0 ~ 59
}SszTime;

typedef struct
{
	int16_t year;//range: SSZ_BASE_YEAR ~ 
	int8_t month;//range: 1 ~ 12
	int8_t day;//range: 1 ~ 31
	int8_t hour;//range: 0 ~ 23
	int8_t minute;//range: 0 ~ 59
	int8_t second;//range: 0 ~ 59
	int8_t weekday;//range: 0~6=Sunday,Monday,...
}SszDateTime;

#ifdef __cplusplus
extern "C" {
#endif
	
//get current time
const SszDateTime* ssz_time_now();
const char* ssz_time_now_str();
//return now time str, no date, e.g. 22:45:30
const char* ssz_time_now_h_str();

//get current seconds elapsed from base time(SSZ_BASE_YEAR.1.1 00:00:00)
int32_t ssz_time_now_seconds(void);

//return the str of time, the time is null, return now time str, e.g. 2017-10-04 22:45:30
const char* ssz_time_str(const SszDateTime *in_time, char buff[SSZ_TIME_STR_SIZE]);

//return the str of time, the time is null, return now time str, e.g. 22:45:30
const char* ssz_time_h_str(const SszTime *in_time, char buff[SSZ_TIME_H_STR_SIZE]);

//like strftime
// Format:
/*Y	writes year as a decimal number, e.g. 2017
*y	writes last 2 digits of year as a decimal number (range [00,99])
*
*b	writes abbreviated month name, e.g. Oct (locale dependent)
*B	writes full month name, e.g. October (locale dependent)
*m	writes month as a decimal number (range [01,12])
*
*d	writes day of the month as a decimal number (range [01,31])
*
*a	writes abbreviated weekday name, e.g. Fri (locale dependent)
*A	writes full weekday name, e.g. Friday (locale dependent)
*w	writes weekday as a decimal number, where Sunday is 0 (range [0-6])
*
*H	writes hour as a decimal number, 24 hour clock (range [00-23])
*I	writes hour as a decimal number, 12 hour clock (range [01,12])
*M	writes minute as a decimal number (range [00,59])
*S	writes second as a decimal number (range [00,60])
*
*c	writes standard date and time string, e.g. Sun Oct 17 04:41:13 2010 (locale dependent)
*D	equivalent to "%m/%d/%y"
*F	equivalent to "%Y-%m-%d" (the ISO 8601 date format)
*R	equivalent to "%H:%M"
*T	equivalent to "%H:%M:%S" (the ISO 8601 time format)
*p	writes localized a.m. or p.m. (locale dependent)
*
* refer http://en.cppreference.com/w/c/chrono/strftime
*return the bytes to write
*/
int ssz_time_format(char* str, int str_size, const char* format, const SszDateTime *time);

#ifdef __cplusplus
}
#endif