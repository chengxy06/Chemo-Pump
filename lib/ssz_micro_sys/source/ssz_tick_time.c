#include "ssz_tick_time.h"
#include "ssz_config.h"
#include "ssz_time_utility.h"
#include "stdio.h"
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
//the time elapse from power on to now, used for count.
//unit: ms
static int32_t g_tick_time_now_ms = 0;
//the value when call ssz_tick_time_now_str,if not same as g_tick_time_now_ms, need update
static int32_t g_tick_time_last_read_str_ms = 0;
static int  g_tick_time_new_tick_flag = 0;
static char g_tick_time_now_str[20];

/************************************************
* Function 
************************************************/

void ssz_tick_time_increase(int ms){
	if(INT32_MAX - g_tick_time_now_ms < ms){
		g_tick_time_now_ms = ms - 1 - (INT32_MAX - g_tick_time_now_ms);
	}else
	{
		g_tick_time_now_ms += ms;
	}

	g_tick_time_new_tick_flag = 1;
}


int32_t ssz_tick_time_next_ex(int32_t curr_ms, int increase_ms) {
	if (INT32_MAX - curr_ms < increase_ms) {
		return increase_ms - 1 - (INT32_MAX - curr_ms);
	}
	else
	{
		return curr_ms+increase_ms;
	}
}

//return the new time by increase the time
int32_t ssz_tick_time_next(int increase_ms) {
	return ssz_tick_time_next_ex(g_tick_time_now_ms, increase_ms);
}

//return the now ms since power on
int32_t ssz_tick_time_now(){
	return g_tick_time_now_ms;
}
const char* ssz_tick_time_now_str() {
	if (g_tick_time_last_read_str_ms==g_tick_time_now_ms) {
		return g_tick_time_now_str;
	}

	g_tick_time_last_read_str_ms = g_tick_time_now_ms;
	ssz_tick_time_str(g_tick_time_last_read_str_ms, g_tick_time_now_str, sizeof(g_tick_time_now_str));

	return g_tick_time_now_str;
}
const char* ssz_tick_time_str(int32_t tick_time, char* str, int str_max_size) {

#if SSZ_TIME_STR_FORMAT_WHEN_PRINT==1
	snprintf(str, str_max_size, "%d.%03d",
		tick_time / 1000, tick_time % 1000);
#elif SSZ_TIME_STR_FORMAT_WHEN_PRINT==2
	int hours;
	int minute;
	int second;
	int ms;
	ms = ssz_milliseconds_to_run_time_of_hours(tick_time, &hours, &minute, &second);
	snprintf(str, str_max_size,
		"%d:%02d:%02d.%03d", hours, minute, second, ms);
#elif SSZ_TIME_STR_FORMAT_WHEN_PRINT==3
	int days;
	int hour;
	int minute;
	int second;
	int ms;
	ms = ssz_milliseconds_to_run_time_of_days(tick_time, &days, &hour, &minute, &second);
	snprintf(str, str_max_size,
		"%d day, %02d:%02d:%02d.%03d", days, hour, minute, second, ms);
#else	
	snprintf(str, str_max_size, "%d", tick_time);
#endif

	return str;
}
//calc the elapsed time from old_ms_time to now, unit: ms, old_ms_time:unit=ms
//set old_ms_time as 0 can get the elapsed time from power on.
int32_t ssz_tick_time_elapsed(int32_t old_ms_time){
	return ssz_tick_time_elapsed_ex(old_ms_time, g_tick_time_now_ms);
}

//calc the elapsed time from old_ms_time to new_ms_time, unit: ms
int32_t ssz_tick_time_elapsed_ex(int32_t old_ms_time, int32_t new_ms_time){
	if(new_ms_time >= old_ms_time){
        return new_ms_time-old_ms_time;
    } else {
        return INT32_MAX-old_ms_time+new_ms_time+1;
    }
}

//get new tick flag, return: 1->exist new tick, 0->not exist  
int ssz_tick_time_get_new_tick_flag(){
	return g_tick_time_new_tick_flag;
}

//clear new tick flag  
void ssz_tick_time_clear_new_tick_flag(){
	g_tick_time_new_tick_flag = 0;
}

bool ssz_tick_time_is_after_or_eq(int32_t a, int32_t b)
{
	int32_t diff = a - b;
	if ( (diff >= 0 && diff < INT32_MAX / 2)||
	(diff<-INT32_MAX/2) ){
		return true;
	}
	return false;
}

//returns the diff between old_ms_time and new_ms_time
//Note: the difference from ssz_tick_time_elapsed is that,
//the interval between old_ms_time and new_ms_time can not >= 24days
//it will auto judge if the new_ms_time is bigger than old_ms_time
//the return value may be negative
int32_t ssz_tick_time_diff(int32_t old_ms_time, int32_t new_ms_time){

	if(ssz_tick_time_is_after_or_eq(new_ms_time,old_ms_time)){
		return ssz_tick_time_elapsed_ex(old_ms_time, new_ms_time);
	}else{
		return -ssz_tick_time_elapsed_ex(new_ms_time, old_ms_time);
	}
}

