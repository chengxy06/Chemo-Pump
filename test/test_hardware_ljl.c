#ifdef TEST
#include "greatest.h"
#include "ssz_time.h"
#include <stdio.h>
#include <stdlib.h>
#include "ssz_common.h"
#include "dev_def.h"
/***********************************************/
//this will call before every case at this suite
static void test_hardware_ljl_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_hardware_ljl_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_rtc() {
}

static void test_hardware_ljl() {
	//printf("set pin high\n");
	//ssz_get_char();
	//GASSERT(ssz_gpio_is_high(SENSOR_PWR_EN_GPIO_Port, SENSOR_PWR_EN_Pin));

}


/***********************************************/
#include "hardware_ljl_suite_def.h"
/***********************************************/
#endif


