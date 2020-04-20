/************************************************
* DESCRIPTION:
*
*
************************************************/
#include "stop_watch.h"
#include "ssz_stop_watch.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
#if STOP_WATCH_MODULE_ENABLE
SszStopWatch g_stop_watch;
SszStopWatchItem g_stop_watch_childs[10];
static bool g_stop_watch_is_enable;

/************************************************
* Function 
************************************************/

void stop_watch_init() {
	ssz_stop_watch_init(&g_stop_watch, g_stop_watch_childs, ssz_array_size(g_stop_watch_childs));
	g_stop_watch_is_enable = true;
}

void stop_watch_start() {
	if (!g_stop_watch_is_enable) {
		return;
	}
	ssz_stop_watch_start(&g_stop_watch);
}

int stop_watch_child_start(const char* name) {
	if (!g_stop_watch_is_enable) {
		return 0;
	}
	return ssz_stop_watch_child_start(&g_stop_watch, name);
}
void stop_watch_child_stop(int index) {
	if (!g_stop_watch_is_enable) {
		return;
	}
	ssz_stop_watch_child_stop(&g_stop_watch, index);
}

void stop_watch_stop() {
	if (!g_stop_watch_is_enable) {
		return;
	}
	ssz_stop_watch_stop(&g_stop_watch);
}

//print the result 
void stop_watch_print() {
	if (!g_stop_watch_is_enable) {
		return;
	}
	ssz_stop_watch_print(&g_stop_watch);
}

//only when stopwatch enable, it will work
void stop_watch_enable() {
	g_stop_watch_is_enable = true;
}
void stop_watch_disable() {
	g_stop_watch_is_enable = false;
}
#endif