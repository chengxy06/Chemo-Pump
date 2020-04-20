#pragma once
#include "ssz_def.h"
#include "ssz_time.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif
//start scan rtc
void rtc_start_scan();
void rtc_stop_scan();

//scan the time from drv rtc
void rtc_scan();

//get current time
const SszDateTime* rtc_get();
//read from hardware
const SszDateTime* rtc_get_directly();

//set date time, the time can not early than base time
//the weekday will auto calc
void rtc_set(const SszDateTime *in_time);

//set time by second count
void rtc_set_by_seconds(int32_t seconds);

//set time, the time can not early than base time
void rtc_set_time(const SszTime *in_time);

//set date, the time can not early than base time
//the weekday will auto calc
void rtc_set_date(const SszDate *in_time);

//init RTC with time if RTC not set
void rtc_init_time_if_not_set(const SszDateTime *in_time);

#ifdef __cplusplus
}
#endif


