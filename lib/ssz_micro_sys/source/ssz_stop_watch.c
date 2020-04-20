/************************************************
* DESCRIPTION:
*
*
************************************************/
#include "ssz_stop_watch.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/
#if SSZ_PRECISION_TICK_TIME_MODULE_ENABLE

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
void ssz_stop_watch_init(SszStopWatch* stop_watch, SszStopWatchItem* items, int item_max_num) {
	stop_watch->items = items;
	stop_watch->max_size = item_max_num;
}

//start 
void ssz_stop_watch_start(SszStopWatch* stop_watch) {
	stop_watch->child_size = 0;
	for (int i = 0; i < stop_watch->max_size; i++) {
		stop_watch->items[i].is_wait_stop = false;
	}
	ssz_precision_tick_time_now(&stop_watch->start_time);
}
int ssz_stop_watch_child_start(SszStopWatch* stop_watch, const char* name) {
	if (stop_watch->child_size < stop_watch->max_size) {
		if (name) {
			snprintf(stop_watch->items[stop_watch->child_size].name, sizeof(stop_watch->items[stop_watch->child_size].name), "%s", name);
		}
		else {
			stop_watch->items[stop_watch->child_size].name[0] = 0;
		}
		stop_watch->items[stop_watch->child_size].is_wait_stop = true;
		ssz_precision_tick_time_now(&stop_watch->items[stop_watch->child_size].start_time);
		stop_watch->child_size++;
		return stop_watch->child_size - 1;
	}
	else {
		return -1;
	}
}
void ssz_stop_watch_child_stop(SszStopWatch* stop_watch, int child_index) {
	if (child_index>=0 && child_index<stop_watch->child_size) {
		ssz_precision_tick_time_now(&stop_watch->items[child_index].stop_time);
		stop_watch->items[child_index].is_wait_stop = false;
	}
}

//stop
void ssz_stop_watch_stop(SszStopWatch* stop_watch) {
	 ssz_precision_tick_time_now(&stop_watch->stop_time);
}
//print the result 
void ssz_stop_watch_print(SszStopWatch* stop_watch) {
	int total_us = ssz_precision_tick_time_elapsed_us_ex(&stop_watch->start_time,
		&stop_watch->stop_time);
	printf("total run time: %0.3fms\n", total_us/1000.0);

	int tmp_us;
	SszPrecisionTickTime* stop_time;
	for (int i = 0; i < stop_watch->child_size; i++) {
		if (stop_watch->items[i].is_wait_stop) {
			stop_time = &stop_watch->stop_time;
		}
		else {
			stop_time = &stop_watch->items[i].stop_time;
		}
		tmp_us = ssz_precision_tick_time_elapsed_us_ex(&stop_watch->items[i].start_time,
			stop_time);
		printf("[%s] run %0.3fms %d%%\n", stop_watch->items[i].name, tmp_us/1000.0, tmp_us * 100 / total_us);
	}
}

#endif
