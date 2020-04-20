/************************************************
* DESCRIPTION:
*   you can custom the function define at this file to
*		let it work right
*
************************************************/
#include "config.h"
#include "ssz_config.h"
#include "ssz_def.h"
#include "ssz_common.h"
#include "ssz_tick_time.h"
#include "assert.h"
#include "stdio.h"
#include "com.h"
#include "ssz_time.h"
#if RTC_MODULE_ENABLE
#include "mid_rtc.h"
#endif
#include "ssz_time_utility.h"
#include "app_common.h"
#include "mid_common.h"

/************************************************
* Declaration
************************************************/
#if RTC_MODULE_ENABLE
#define USE_SYSTICK_TO_GET_TIME 0
#else
#define USE_SYSTICK_TO_GET_TIME 1
#endif

#define CLEAR_WATCHDOG_MIN_INTERVAL_MS  50

/************************************************
* Variable 
************************************************/
#if USE_SYSTICK_TO_GET_TIME
SszDateTime g_date_time_from_systick;
#endif
int g_last_clear_watchdog_time_point=0;

/************************************************
* Function 
************************************************/

//get current time
const SszDateTime* ssz_time_now() {
#if USE_SYSTICK_TO_GET_TIME
	ssz_seconds_to_time(ssz_tick_time_now() / 1000, &g_date_time_from_systick);
	return &g_date_time_from_systick;
#else
	return rtc_get();
#endif
}

//get current seconds elapsed from base time(SSZ_BASE_YEAR.1.1 00:00:00)
//default we use systick to calc current time, if have RTC, need change to use RTC
int32_t ssz_time_now_seconds(void) {
#if USE_SYSTICK_TO_GET_TIME
	return ssz_tick_time_now() / 1000;
#else
	SszDateTime out_time;
	out_time = *ssz_time_now();
	return ssz_time_to_seconds(&out_time);
#endif
}
//return the now time, used at ssz_print, ssz_traceln
const char* ssz_now_time_str_for_print() {
#if SSZ_TIME_STR_FORMAT_WHEN_PRINT>=0&&SSZ_TIME_STR_FORMAT_WHEN_PRINT<=3
	return ssz_tick_time_now_str();
#elif SSZ_TIME_STR_FORMAT_WHEN_PRINT==4
	return ssz_time_now_h_str();
#elif SSZ_TIME_STR_FORMAT_WHEN_PRINT==5
	return ssz_time_now_str();
#else
#error "not support format"
#endif
}
//if enable watchdog, need implement it
void ssz_clear_watchdog()
{
	if (watchdog_is_enable() &&
		ssz_tick_time_elapsed(g_last_clear_watchdog_time_point) >= CLEAR_WATCHDOG_MIN_INTERVAL_MS) {
		watchdog_clear();
		//ssz_traceln("clear_watchdog" );
		g_last_clear_watchdog_time_point = ssz_tick_time_now();
	}

}

//read a char from terminal
char ssz_get_char() {
	SszFile* fl = com_to_file(kComPCUart);
	return ssz_fgetc(fl);
}


#ifdef DEBUG
//call it when assert fail by ssz_assert, need hold or terminate system
void ssz_on_assert_fail(const char* filename, int lineno, const char* expression)
{
	printf("%s:%d %s -- Assert Fail\n", filename, lineno, expression);
	assert(0);//lint !e506
}
//call it when check fail by ssz_check, need hold or terminate system
void ssz_on_check_fail(const char* filename, int lineno, const char* expression)
{
	printf("%s:%d %s -- Check Fail\n", filename, lineno, expression);
	assert(0);//lint !e506
	while (1);
}
#else
//call it when check fail by ssz_check, need hold or terminate system
void ssz_on_check_fail()
{
	printf("check fail!\n");
	assert(0);//lint !e506
	while (1);
}
#endif

//abort system
void ssz_abort()
{
	printf("abort\n");
	assert(0);//lint !e506
	while(1);
}
