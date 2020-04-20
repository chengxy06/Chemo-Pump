#include "ssz_time.h"
#include "ssz_time_utility.h"
#include "mid_rtc.h"
#include "timer.h"
#include "msg.h"
#include "drv_rtc.h"
#include "string.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "sim_interface.h"
#endif
#include "config.h"
#include "stdio.h"
#include "alarm.h"
#include "common.h"
#include "record_log.h"
#include "ssz_tick_time.h"

/************************************************
* Declaration
************************************************/
#if RTC_MODULE_ENABLE
#define RTC_SCAN_TIME_MS 20
#define RTC_CHECK_PERIOD_TIME_MS 3000

/************************************************
* Variable 
************************************************/
static SszDateTime g_rtc_time;
static SszDateTime g_rtc_time_check;
static int32_t g_rtc_check_last_time_ms;
static int g_rtc_recheck_count = 0;

/************************************************
* Function 
************************************************/

//start scan rtc
void rtc_start_scan( )
{
    timer_set_handler(kTimerScanRTC, rtc_scan);
	timer_start_periodic_every(kTimerScanRTC, RTC_SCAN_TIME_MS);
	rtc_scan();

    g_rtc_check_last_time_ms = ssz_tick_time_now();
}

void rtc_stop_scan()
{
	timer_stop(kTimerScanRTC);
}

void rtc_scan( )
{
	SszDateTime rtc;

#ifdef SSZ_TARGET_MACHINE
	drv_rtc_get_time_all(&rtc);
#else
	ssz_seconds_to_time(sim_rtc_get_seconds(), &rtc);
#endif
	if (memcmp(&rtc, &g_rtc_time, sizeof(g_rtc_time)) != 0) {
		g_rtc_time = rtc;
		if (!msg_is_exist(kMsgTimeChanged)) {
			msg_post_int(kMsgTimeChanged, 0);
		}
	}
    //check if rtc is working, time goes on
    if (ssz_tick_time_elapsed(g_rtc_check_last_time_ms) >= RTC_CHECK_PERIOD_TIME_MS) {
        g_rtc_check_last_time_ms = ssz_tick_time_now();
        
        if (memcmp(&rtc, &g_rtc_time_check, sizeof(g_rtc_time_check)) == 0 ) {
            if (g_rtc_recheck_count < 2) {
                drv_rtc_start();
                g_rtc_recheck_count++;
            } else {
                g_rtc_recheck_count = 0;
			    common_printfln("rtc error!\n");;	
			    alarm_set(kAlarmRTCErrorID);
            }
    	}
        g_rtc_time_check = rtc;
    }

}

const SszDateTime* rtc_get()
{
	return &g_rtc_time;
}

const SszDateTime* rtc_get_directly()
{
	rtc_scan();
	return &g_rtc_time;
}

//set date time, the time can not early than base time
//the weekday will auto calc
void rtc_set(const SszDateTime *in_time) {
	SszDateTime tmp;
	tmp = *in_time;

	tmp.weekday = ssz_week_day(tmp.year, tmp.month, tmp.day);
#if SSZ_TARGET_MACHINE
	drv_rtc_set_time_all(&tmp);
#else
	sim_rtc_set_seconds(ssz_time_to_seconds(in_time));
#endif
	rtc_scan();
}

//set time by second count
void rtc_set_by_seconds(int32_t seconds) {
	SszDateTime tmp;
	ssz_seconds_to_time(seconds, &tmp);
	rtc_set(&tmp);
}

//set time, the time can not early than base time
void rtc_set_time(const SszTime *in_time) {
	SszDateTime out_time;
	out_time = g_rtc_time;
	out_time.hour = in_time->hour;
	out_time.minute= in_time->minute;
	out_time.second = in_time->second;
	rtc_set(&out_time);
}

//set date, the time can not early than base time
void rtc_set_date(const SszDate *in_time) {
	SszDateTime out_time;
	out_time = g_rtc_time;
	out_time.year = in_time->year;
	out_time.month = in_time->month;
	out_time.day = in_time->day;
	rtc_set(&out_time);
}

//init RTC with time if RTC not set
void rtc_init_time_if_not_set(const SszDateTime *in_time) {
	SszTime temp;
	temp.hour = 0 ;
	temp.minute = 0 ;
	temp.second = 1 ;	
#if SSZ_TARGET_MACHINE  
	drv_rtc_get_time_all(&g_rtc_time);
//if((g_rtc_time.year == 2001)&&
//	(g_rtc_time.month == 1)&&
//	(g_rtc_time.day == 1)&&
//	(g_rtc_time.hour == 0)&&
//	(g_rtc_time.minute == 0)&&
//	(g_rtc_time.second == 0))
//	{
//		rtc_set_time(&temp);		
//	}
if((g_rtc_time.year == 2048)||
	(g_rtc_time.month < 1)||
	(g_rtc_time.month > 12)||
	(g_rtc_time.day < 1)||
	(g_rtc_time.day > 31)||	
	(g_rtc_time.hour < 0)||
	(g_rtc_time.hour > 23)||
	(g_rtc_time.minute < 0)||
	(g_rtc_time.minute > 59))
	{
		g_rtc_time.year = 2017;
		g_rtc_time.month = 6;
		g_rtc_time.day = 1;
		g_rtc_time.hour = 9;
		g_rtc_time.minute = 6;
		g_rtc_time.second = 0;
		rtc_set_time(&temp);		
	}
#else
	SIMDateTime info = sim_get_now_time();
	memcpy(&g_rtc_time,(SszDateTime*)&info, sizeof(g_rtc_time));
	sim_rtc_set_seconds(ssz_time_to_seconds((SszDateTime*)&info));
#endif
}
#endif