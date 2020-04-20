#ifdef TEST
#include "greatest.h"
#include "ssz_str_utility.h"
#include <stdio.h>
#include <stdlib.h>
/***********************************************/
//this will call before every case at this suite
static void test_ssz_str_utility_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_ssz_str_utility_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_ssz_str_find_number() {
	char sz[] = "p12 45i56_78oiii";
	const char* p = sz;
	p = ssz_str_find_number(p);
	GASSERT(atoi(p) == 12);
	p = ssz_str_find_number(p+2);
	GASSERT(atoi(p) == 45);
	p = ssz_str_find_number(p + 2);
	GASSERT(atoi(p) == 56);
	p = ssz_str_find_number(p + 2);
	GASSERT(atoi(p) == 78);
	p = ssz_str_find_number(p + 2);
	GASSERT(p == NULL);
}

static void test_ssz_str_find_not_number() {
	char sz[] = "p12 45i56_78o";
	const char* p = sz;
	p = ssz_str_find_not_number(p);
	GASSERT(*p == 'p');
	p++;
	p = ssz_str_find_not_number(p);
	GASSERT(*p == ' ');

	p++;
	p = ssz_str_find_not_number(p);
	GASSERT(*p == 'i');

	p++;
	p = ssz_str_find_not_number(p);
	GASSERT(*p == '_');

	p++;
	p = ssz_str_find_not_number(p);
	GASSERT(*p == 'o');

	p++;
	p = ssz_str_find_not_number(p);
	GASSERT(p == NULL);

}

static void test_ssz_str_to_numbers() {
	char sz[] = "p12 45i56_78o";
	int nums[7];
	GASSERT(ssz_str_to_numbers(sz, nums, 1)==1);
	GASSERT(nums[0] == 12);
	GASSERT(ssz_str_to_numbers(sz, nums, 7) == 4);
	GASSERT(nums[0] == 12);
	GASSERT(nums[1] == 45);
	GASSERT(nums[2] == 56);
	GASSERT(nums[3] == 78);

}

static void test_ssz_str_split() {
	char sz1[] = "p12 45i56_78o";
	char sz[20];
	char *params[10];
	int param_size = 0;
	strcpy(sz, sz1);
	GASSERT(ssz_str_split(sz, " ", params, 10) == 2);
	strcpy(sz, sz1);
	GASSERT(ssz_str_split(sz, "46", params, 10) == 3);
	strcpy(sz, sz1);
	GASSERT(ssz_str_split(sz, "15", params, 10) == 4);

}

static void test_ssz_hex_to_int() {
	GASSERT(ssz_str_hex_to_int("0x12345678") == 0x12345678);
	GASSERT(ssz_str_hex_to_int("A0") == 0xA0);
	GASSERT(ssz_str_hex_to_int("0AG") == 0xA);
}

/***********************************************/
#include "ssz_str_utility_suite_def.h"
/***********************************************/
#endif


