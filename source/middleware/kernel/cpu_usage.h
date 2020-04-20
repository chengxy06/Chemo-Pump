/************************************************
* DESCRIPTION:
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "config.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif
#if CPU_USAGE_MODULE_ENABLE

//when call it, it will reset usage
void cpu_usage_print_if_large_than(int compare_usage);
void cpu_usage_reset_usage();
void cpu_usage_enter_idle();
void cpu_usage_exit_idle();

#endif
#ifdef __cplusplus
}
#endif

