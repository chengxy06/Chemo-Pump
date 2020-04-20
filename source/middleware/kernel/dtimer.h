/************************************************
* DESCRIPTION:
*
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "ssz_timer.h"
#include "config.h"
/************************************************
* Declaration
************************************************/
#if DTIMER_MODULE_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

void dtimer_start_scan_timer();
void dtimer_stop_scan_timer();

//return the timer
//	-1: create fail
SszTimer dtimer_create(SszTimerHandler func, void* user_data);
void dtimer_delete(SszTimer timer);

//start a timer(previous timer will be clear),
//when milliseconds from now is pass, the handle function will be called.
//Note: the timer only run once,
void dtimer_start_oneshot_after(SszTimer timer, int32_t milliseconds);

//stop the timer
void dtimer_stop(SszTimer timer);




#ifdef __cplusplus
}
#endif

#endif