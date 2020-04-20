#ifdef TEST
#include "greatest.h"
#include "drv_rtc.h"
#include <stdio.h>
#include <stdlib.h>

int time_tmp;
SszDateTime time_set={2020,10,16,11,20,5,2};

/***********************************************/
//this will call before every case at this suite
static void test_drv_rtc_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_drv_rtc_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_drv_rtc_init() {
}

static void test_drv_rtc_start() {
  //drv_rtc_start();
}

static void test_drv_rtc_stop() {
}

static void test_drv_rtc_set_time_individually() {
  drv_rtc_init();
  drv_rtc_set_time_individually(kRtcSec,10);
  drv_rtc_set_time_individually(kRtcMin,23);
  drv_rtc_set_time_individually(kRtcHour,14);
  drv_rtc_set_time_individually(kRtcWkday,3);
  drv_rtc_set_time_individually(kRtcDate,17);
  drv_rtc_set_time_individually(kRtcMth,9);
  drv_rtc_set_time_individually(kRtcYear,2019);
  
  drv_rtc_start();
}

static void test_drv_rtc_get_time_individually() 
{
  time_tmp=drv_rtc_get_time_individually(kRtcSec);
  
  int i;
  for(i=0; i<10;i++){
    time_tmp=drv_rtc_get_time_individually(kRtcSec);
  }

  time_tmp=drv_rtc_get_time_individually(kRtcSec);
  time_tmp=drv_rtc_get_time_individually(kRtcMin);
  time_tmp=drv_rtc_get_time_individually(kRtcHour);
  time_tmp=drv_rtc_get_time_individually(kRtcWkday);
  time_tmp=drv_rtc_get_time_individually(kRtcMth);
  time_tmp=drv_rtc_get_time_individually(kRtcDate);
  time_tmp=drv_rtc_get_time_individually(kRtcYear);
}

static void test_drv_rtc_set_time_all()
{
  drv_rtc_set_time_all(&time_set);
  drv_rtc_start();
}

static void test_drv_rtc_get_time_all() 
{
  int i;
  time_tmp=drv_rtc_get_time_individually(kRtcSec);
  time_tmp=drv_rtc_get_time_individually(kRtcMin);
  time_tmp=drv_rtc_get_time_individually(kRtcHour);
  time_tmp=drv_rtc_get_time_individually(kRtcWkday);
  time_tmp=drv_rtc_get_time_individually(kRtcMth);
  time_tmp=drv_rtc_get_time_individually(kRtcDate);
  time_tmp=drv_rtc_get_time_individually(kRtcYear);

  for(i=0; i<10;i++){
    drv_rtc_stop();
    drv_rtc_get_time_all(&time_set);
    drv_rtc_get_time_all(&time_set);
    drv_rtc_start();
    drv_rtc_get_time_all(&time_set);
    drv_rtc_get_time_all(&time_set);
  }
    drv_rtc_get_time_all(&time_set);
}


/***********************************************/
#ifdef SSZ_TARGET_MACHINE
#include "drv_rtc_suite_def.h"
#else
void suite_drv_rtc() {

}
#endif
#endif


