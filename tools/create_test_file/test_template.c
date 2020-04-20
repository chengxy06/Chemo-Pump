#ifdef TEST
#include "greatest.h"
#include "{file_name}.h"
#include <stdio.h>
#include <stdlib.h>
/***********************************************/
//this will call before every case at this suite
static void test_{file_name}_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_{file_name}_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
{case_define}
/***********************************************/
#include "{file_name}_suite_def.h"
#endif


