#ifdef TEST
#include "greatest.h"
#include "ssz_time_utility.h"
#include <stdio.h>
#include <stdlib.h>
/***********************************************/
//this will call before every case at this suite
static void test_ssz_time_utility_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_ssz_time_utility_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_ssz_is_leap_year() {
}

static void test_ssz_max_days_in_month() {
	GASSERT(ssz_max_days_in_month(2017, 1) == 31);
	GASSERT(ssz_max_days_in_month(2017, 2) == 28);
	GASSERT(ssz_max_days_in_month(2016, 2) == 29);
}

static void test_ssz_week_day() {
	int week = 0;
	int month = 1;
	int day = 1;
	for (int i = 0; i < 365; i++) {
		GASSERT(ssz_week_day(2017, month, day) == week);
		week++;
		if (week>6) {
			week = 0;
		}
		day++;
		if (day>ssz_max_days_in_month(2017, month)) {
			day = 1;
			month++;
		}
	}
}

static void test_ssz_time_to_seconds() {
}

static void test_ssz_time_to_seconds_ex() {
}

static void test_ssz_seconds_to_time() {
}

static void test_ssz_seconds_to_time_ex() {
}

static void test_ssz_str_to_date() {
	SszDate a = ssz_str_to_date("2017-01-02", kSszDateFormat_YYYY_MM_DD);
	GASSERT(a.year == 2017 && a.month == 1 && a.day == 2);
	a = ssz_str_to_date("02/01/2017", kSszDateFormat_DD_MM_YYYY);
	GASSERT(a.year == 2017 && a.month == 1 && a.day == 2);
	a = ssz_str_to_date("01/02, 2017", kSszDateFormat_MM_DD_YYYY);
	GASSERT(a.year == 2017 && a.month == 1 && a.day == 2);
}

static void test_ssz_str_to_time() {
	SszTime a = ssz_str_to_time("18:20:34");
	GASSERT(a.hour == 18 && a.minute == 20 && a.second == 34);
}


/***********************************************/
#include "ssz_time_utility_suite_def.h"
/***********************************************/
#endif


