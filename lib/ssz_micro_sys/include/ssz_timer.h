#pragma once
#include "ssz_def.h"
#include "ssz_list.h"

/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif
typedef int SszTimer;
typedef void(*SszTimerHandler)(SszTimer timer_id, void* user_data);
typedef struct
{
	int32_t expire_time; //when expire_time is arrived, call the handle
	SszTimerHandler handle_func;
	void* user_data;
	bool is_activated; //is timer is activated
}SszTimerObj;
typedef struct
{
	SSZ_LIST_NODE_BASE_DEFINE;
	SszTimerObj obj;
}SszTimerNode;

typedef SszList SszTimerManager;


void ssz_timer_init(SszTimerManager* timer_manager, SszTimerNode* obj_array, int obj_count);

//return the timer, 
//	-1: create fail
SszTimer ssz_timer_create(SszTimerManager* timer_manager, SszTimerHandler func, void* user_data);
void ssz_timer_delete(SszTimerManager* timer_manager, SszTimer timer);

//start a timer(previous timer will be clear),
//when milliseconds from now is pass, the handle function will be called.
//Note: the timer only run once,
void ssz_timer_start_oneshot_after(SszTimerManager* timer_manager, SszTimer timer, int32_t milliseconds);

//stop the timer
void ssz_timer_stop(SszTimerManager* timer_manager, SszTimer timer);

//handle all timer
void ssz_timer_handle_all(SszTimerManager* timer_manager);

#ifdef __cplusplus
}
#endif