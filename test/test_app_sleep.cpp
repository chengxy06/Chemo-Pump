#ifdef TEST
#include "greatest.h"
#include "app_sleep.h"
#include <stdio.h>
#include <stdlib.h>
#include "mid_rtc.h"
#include "ssz_time.h"
#include "ssz_time_utility.h"
#include "ssz_common.h"
/***********************************************/
//this will call before every case at this suite
static void test_app_sleep_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_app_sleep_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_app_sleep_enter() {
}

static void test_app_sleep_enter_and_exit_at() {
	SszDateTime date;
	int32_t curr_secs;
	int32_t last_secs;
	for(int i=0; i<10; i++){
		rtc_scan();
		curr_secs = ssz_time_now_seconds();
		last_secs = curr_secs;
		curr_secs+=60;
		ssz_seconds_to_time(curr_secs, &date);
		ssz_traceln("RTC Time:%s, will sleep 60s", ssz_time_now_str());
		app_sleep_enter_and_exit_at(&date);
		rtc_scan();
		ssz_traceln("RTC Time:%s, wake up, the deviation is %ds", 
			ssz_time_now_str(), ssz_time_now_seconds()-last_secs-60);
	}
}

static void test_app_sleep_enter_for_ms() {
}

static void test_app_sleep_exit() {
}


/***********************************************/
#include "app_sleep_suite_def.h"
#endif


