/************************************************
* DESCRIPTION:
*	dynamic timer
*
************************************************/
#include "dtimer.h"
#include "timer.h"
#include "ssz_common.h"

#if DTIMER_MODULE_ENABLE
/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static SszTimerManager g_dtimer_manager;
static SszTimerNode g_dtimer_manager_nodes[DTIMER_MAX_COUNT];

/************************************************
* Function 
************************************************/

void dtimer_handle_all_timer() {
	ssz_timer_handle_all(&g_dtimer_manager);
}

void dtimer_start_scan_timer(){
	ssz_timer_init(&g_dtimer_manager, g_dtimer_manager_nodes,
		ssz_array_size(g_dtimer_manager_nodes));
	timer_set_handler(kTimerHandleDTimer, dtimer_handle_all_timer);
	timer_start_periodic_every(kTimerHandleDTimer, MSECS_PER_TICK);
	dtimer_handle_all_timer();
}

void dtimer_stop_scan_timer()
{
	timer_stop(kTimerHandleDTimer);
}


//return the timer
//	-1: create fail
SszTimer dtimer_create(SszTimerHandler func, void* user_data){
	return ssz_timer_create(&g_dtimer_manager, func, user_data);
}
void dtimer_delete(SszTimer timer){
	ssz_timer_delete(&g_dtimer_manager, timer);
}

//start a timer(previous timer will be clear),
//when milliseconds from now is pass, the handle function will be called.
//Note: the timer only run once,
void dtimer_start_oneshot_after(SszTimer timer, int32_t milliseconds){
	ssz_timer_start_oneshot_after(&g_dtimer_manager, timer, milliseconds);
}

//stop the timer
void dtimer_stop(SszTimer timer){
	ssz_timer_stop(&g_dtimer_manager, timer);
}


#endif