/************************************************
* DESCRIPTION:
*
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "ssz_precision_tick_time.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif
#if SSZ_PRECISION_TICK_TIME_MODULE_ENABLE
	typedef struct {
		SszPrecisionTickTime start_time;
		SszPrecisionTickTime stop_time;
		char name[20];
		bool is_wait_stop;
	}SszStopWatchItem;
typedef struct {
	SszPrecisionTickTime start_time;
	SszPrecisionTickTime stop_time;
	SszStopWatchItem* items;
	int child_size;
	int max_size;
}SszStopWatch;

void ssz_stop_watch_init(SszStopWatch* stop_watch, SszStopWatchItem* items, int item_max_num);
//start 
void ssz_stop_watch_start(SszStopWatch* stop_watch);

//return child index
int ssz_stop_watch_child_start(SszStopWatch* stop_watch, const char* name);
void ssz_stop_watch_child_stop(SszStopWatch* stop_watch, int child_index);

//stop
void ssz_stop_watch_stop(SszStopWatch* stop_watch);
//print the result 
void ssz_stop_watch_print(SszStopWatch* stop_watch);
#endif
#ifdef __cplusplus
}
#endif

