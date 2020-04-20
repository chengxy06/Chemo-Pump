#pragma once
#include "ssz_def.h"

/************************************************
* Declaration
************************************************/


#ifdef __cplusplus
extern "C" {
#endif

void ssz_tick_time_increase(int ms);

//return the new time by increase the time to curr_ms
int32_t ssz_tick_time_next_ex(int32_t curr_ms, int increase_ms);

//return the new time by increase the time
int32_t ssz_tick_time_next(int increase_ms);

//return the now ms since power on
//Note: it support max 24 days, after 24days, it will recount from 0
int32_t ssz_tick_time_now();
//return the str according SSZ_TIME_STR_FORMAT_WHEN_PRINT
const char* ssz_tick_time_now_str();
const char* ssz_tick_time_str(int32_t tick_time, char* str, int str_max_size);

//calc the elapsed time from old_ms_time to now, unit: ms, old_ms_time:unit=ms
//set old_ms_time as 0 can get the elapsed time from power on.
int32_t ssz_tick_time_elapsed(int32_t old_ms_time);

//calc the elapsed time from old_ms_time to new_ms_time, unit: ms
int32_t ssz_tick_time_elapsed_ex(int32_t old_ms_time, int32_t new_ms_time);

//get new tick flag, return: 1->exist new tick, 0->not exist
int ssz_tick_time_get_new_tick_flag();

//clear new tick flag
void ssz_tick_time_clear_new_tick_flag();

//returns true if the time a is after or equal time b
bool ssz_tick_time_is_after_or_eq(int32_t a, int32_t b);

//returns the diff between old_ms_time and new_ms_time
//Note: the difference from ssz_tick_time_elapsed is that,
//the interval between old_ms_time and new_ms_time can not >= 24days
//it will auto judge if the new_ms_time is bigger than old_ms_time
//the return value may be negative
int32_t ssz_tick_time_diff(int32_t old_ms_time, int32_t new_ms_time);

#ifdef __cplusplus
}
#endif