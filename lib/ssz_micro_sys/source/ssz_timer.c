#include "ssz_timer.h"
#include "ssz_common.h"
#include "ssz_tick_time.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/


/************************************************
* Function 
************************************************/

void ssz_timer_init(SszTimerManager * timer_manager, SszTimerNode * obj_array, int obj_count)
{
	ssz_mem_zero(timer_manager, sizeof(*timer_manager));
	ssz_list_init(timer_manager, (SszListNode*)obj_array, obj_count, sizeof(SszTimerNode));
}

SszTimer ssz_timer_create(SszTimerManager * timer_manager, SszTimerHandler func, void * user_data)
{
	SszTimerObj *obj;
	obj = ssz_list_emplace_back(timer_manager);
	if (obj) {
		ssz_mem_zero(obj, sizeof(*obj));
		obj->handle_func = func;
		obj->user_data = user_data;
		return ssz_list_rbegin(timer_manager);
	}
	return -1;
}

void ssz_timer_delete(SszTimerManager * timer_manager, SszTimer timer)
{
	ssz_list_erase(timer_manager, timer);
}

void ssz_timer_start_oneshot_after(SszTimerManager * timer_manager, SszTimer timer, int32_t milliseconds)
{
	SszTimerObj *obj = ssz_list_at_pos(timer_manager, timer);
	if (obj) {
		obj->expire_time = ssz_tick_time_next(milliseconds);
		obj->is_activated = true;
	}
	else {
		ssz_assert_fail();
	}
}

void ssz_timer_stop(SszTimerManager * timer_manager, SszTimer timer)
{
	SszTimerObj *obj = ssz_list_at_pos(timer_manager, timer);
	if (obj) {
		obj->is_activated = false;
	}
	else {
		ssz_assert_fail();
	}
}

void ssz_timer_handle_all(SszTimerManager * timer_manager)
{
	int32_t curr_time = ssz_tick_time_now();
	for (int i = ssz_list_begin(timer_manager); i != -1; i=ssz_list_next(timer_manager, i)) {
		SszTimerObj *obj = ssz_list_at_pos(timer_manager, i);
		//if timer is active and expire, clear the timer and call the handle
		if (obj->is_activated &&
			ssz_tick_time_is_after_or_eq(curr_time, obj->expire_time) )
		{
			obj->is_activated = false;
			//call handle func
			if (obj->handle_func)
			{
				obj->handle_func(i, obj->user_data);
			}
		}
	}
}
