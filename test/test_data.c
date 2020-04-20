#ifdef TEST
#include "greatest.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
/***********************************************/
//this will call before every case at this suite
static void test_data_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_data_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_data_nvram() {
}

static void test_data_read() {
}

static void test_data_write() {
}

static void test_data_read_int() {
}

static void test_data_read_uint() {
}
void write_full_of_fct_data() {
	for (int i = 0; i < 4096; i++) {
		data_write_int(kDataBatteryRedress, 10);
		data_flush();
	}
}
void write_full_of_factory_data() {
	for (int i = 0; i < 4096; i++) {
		data_write_int(kDataOcclusionThreshold, 20);
		data_flush();
	}
}
void write_full_of_user_data() {
	for (int i = 0; i < 4096; i++) {
		data_write_int(kDataLastShutdownIsAbnormal, 20);
		data_flush();
	}
}
void write_full_of_infusion_data() {
	for (int i = 0; i < 4096; i++) {
		char buff[10] = "123456789";
		data_write(kDataInfusionInfoBlock, buff, 9);
		data_flush();
	}
}
void write_full_of_para_data() {
	for (int i = 0; i < 4096; i++) {
		char buff[10] = "123456789";
		data_write(kDataSetInfusionParaInfoBlock, buff, 9);
		data_flush();
	}
}
static void test_data_write_int() {
	//write_full_of_para_data();
}

static void test_data_erase_block() {
}

static void test_data_flush() {
}

static void test_data_find_save_info_by_id() {
}

static void test_data_init() {
}


/***********************************************/
#include "data_suite_def.h"
#endif


