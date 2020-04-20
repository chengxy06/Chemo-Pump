#ifdef TEST
#include "greatest.h"
#include "ssz_time.h"
#include <stdio.h>
#include <stdlib.h>
/***********************************************/
//this will call before every case at this suite
static void test_ssz_time_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_ssz_time_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_ssz_time_now() {
}

static void test_ssz_time_now_seconds() {
}

static void test_ssz_time_str() {
	SszDateTime  tim;
	tim.year = 2017;
	tim.month = 2;
	tim.day = 23;
	tim.hour = 8;
	tim.minute = 25;
	tim.second = 45;
	tim.weekday = 1;
	char buff[20];
	//printf("curr time:%s\n",ssz_time_str(NULL));
	ssz_time_str(&tim, buff);
	GASSERT(strcmp(ssz_time_str(&tim, buff), "2017-02-23 08:25:45") == 0);
}

static void test_ssz_time_format() {
	SszDateTime  tim;
	tim.year = 2017;
	tim.month = 2;
	tim.day = 23;
	tim.hour = 8;
	tim.minute = 25;
	tim.second = 45;
	tim.weekday = 1;
	char str[40];
	
	ssz_time_format(str, sizeof(str), "%Y-%m-%d %H:%M:%S", &tim);
	GASSERT(strcmp(str, "2017-02-23 08:25:45") == 0);

	ssz_time_format(str, sizeof(str), "%Y-%m-%d %H:%M:%S %w week", &tim);
	GASSERT(strcmp(str, "2017-02-23 08:25:45 1 week") == 0);

	ssz_time_format(str, sizeof(str), "%c", &tim);
	printf("curr compile time str:%s\n", str);
	//GASSERT(strcmp(str, "2017-02-23 08:25:45") == 0);

	ssz_time_format(str, sizeof(str), "%F %T", &tim);
	GASSERT(strcmp(str, "2017-02-23 08:25:45") == 0);
}


/***********************************************/
#include "ssz_time_suite_def.h"
/***********************************************/
#endif


