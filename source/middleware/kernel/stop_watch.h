/************************************************
* DESCRIPTION:
*
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
#if STOP_WATCH_MODULE_ENABLE


void stop_watch_init();
void stop_watch_start();

int stop_watch_child_start(const char* name);
void stop_watch_child_stop(int index);

void stop_watch_stop();

//print the result 
void stop_watch_print();

//only when stopwatch enable, it will work
void stop_watch_enable();
void stop_watch_disable();


#endif
#ifdef __cplusplus
}
#endif

