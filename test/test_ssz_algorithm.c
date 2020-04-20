#ifdef TEST
#include "greatest.h"
#include "ssz_algorithm.h"
#include <stdio.h>
#include <stdlib.h>
/***********************************************/
//this will call before every case at this suite
static void test_ssz_algorithm_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_ssz_algorithm_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_ssz_crc32() {
	char sz[] = "abcdefghij";
	GASSERT(ssz_crc32(sz, 10) == 0x3981703A);
}

static void test_ssz_crc32_ex() {
	char sz[] = "abcdefghij";
	uint32_t crc32_value;
	crc32_value = ssz_crc32_ex(sz, 5, SSZ_CRC32_SEED);
	crc32_value = ssz_crc32_ex(sz+5, 5, crc32_value);
	crc32_value ^= SSZ_CRC32_SEED;
	GASSERT(crc32_value == 0x3981703A);
}


/***********************************************/
#include "ssz_algorithm_suite_def.h"
#endif


