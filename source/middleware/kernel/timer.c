/************************************************
* DESCRIPTION:
*   
************************************************/
#include "timer.h"
#include "ssz_tick_time.h"
#include "ssz_common.h"


/************************************************
* Declaration
************************************************/
typedef struct  
{
    int32_t expire_time; //when expire_time is arrived, call the handle
    int32_t timer_size;// the timer's size, unit:ms
    TimerHandler handle_func;
    bool is_activated; //is timer is activated
    bool is_repeat; // is timer is repeat
	uint8_t group;
}TimerInfo;

/************************************************
* Variable 
************************************************/
//timer manager
static TimerInfo g_timer_mgr[kTimerIDMax];

/************************************************
* Function 
************************************************/

//set handler of timer id, the timer group is in TIMER_GROUP_NORMAL
void timer_set_handler(TimerID id, TimerHandler func){
    timer_set_handler_and_group(id, func, TIMER_GROUP_NORMAL);
}
void timer_set_handler_and_group(TimerID id, TimerHandler func, uint8_t timer_group)
{
    ssz_assert(id<kTimerIDMax);
    g_timer_mgr[id].handle_func = func;
    g_timer_mgr[id].group = timer_group;
}

/***********************************************
* Description:
*   start a timer(previous timer will be clear),
*   when milliseconds from now is pass, the handle function will be called.
*   Note: the timer only run once,
*   if you want to run again, please call it again at handle function.
*
************************************************/
void timer_start_oneshot_after(TimerID id, int32_t milliseconds)
{
    ssz_assert(milliseconds > 0);
    ssz_assert( id<kTimerIDMax);
    
    #if MSECS_PER_TICK!=1
    //the timer value must multiples of one tick.
    if(milliseconds%MSECS_PER_TICK != 0)
    {
       milliseconds += MSECS_PER_TICK - milliseconds%MSECS_PER_TICK; 
    }
    #endif

    //set the expire time
    g_timer_mgr[id].expire_time = ssz_tick_time_next(milliseconds);

    //active the timer
    g_timer_mgr[id].is_repeat = false;
    g_timer_mgr[id].is_activated = true;
}

/***********************************************
*
* Description:
*   same as timer_start_oneshot_after,but it runs repeat,
*   the handle function will be called each milliseconds.
*   if you want to stop, please call timer_stop.
************************************************/
void timer_start_periodic_every(TimerID id, int32_t milliseconds)
{
    //active the timer
	timer_start_oneshot_after(id, milliseconds);
    //set it as repeat
    g_timer_mgr[id].timer_size = milliseconds;
    g_timer_mgr[id].is_repeat = true;
}

//start a timer(previous timer will be clear),
//when tick_time is arrive, the handle function will be called.
//Note: the timer only run once,
void timer_start_oneshot_at(TimerID id, int32_t tick_time) {
	ssz_assert(id<kTimerIDMax);
	//set the expire time
	g_timer_mgr[id].expire_time = tick_time;

	//active the timer
	g_timer_mgr[id].is_repeat = false;
	g_timer_mgr[id].is_activated = true;
}
/***********************************************
*
* Description:
*   stop the timer
*
************************************************/
void timer_stop(TimerID id)
{
    ssz_assert(id<kTimerIDMax);
    //deactive the timer
    g_timer_mgr[id].is_activated = false;
    g_timer_mgr[id].is_repeat = false;
}

/***********************************************
*
* Description:
*   check if the timer active
*
************************************************/
bool timer_is_actived(TimerID id)
{
    ssz_assert(id<kTimerIDMax);
    if(g_timer_mgr[id].is_activated)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/***********************************************
*
* Description:
*   stop all timer which's group is the timer_group
*
************************************************/
void timer_stop_all(uint8_t timer_group)
{
    int32_t i;
    for (i=0; i<kTimerIDMax; i++)
    {
		if (g_timer_mgr[i].group & timer_group) {
			//clear timer
			g_timer_mgr[i].is_activated = false;
			g_timer_mgr[i].is_repeat = false;
		}

    }
}

//stop and clear all timer handler which's group is the timer_group
void timer_stop_and_clear_all_handler(uint8_t timer_group) {
	int32_t i;
	for (i = 0; i < kTimerIDMax; i++)
	{
		if (g_timer_mgr[i].group & timer_group) {
			//clear timer
			g_timer_mgr[i].is_activated = false;
			g_timer_mgr[i].is_repeat = false;
			g_timer_mgr[i].handle_func = NULL;
		}

	}
}

//handle timer event
void timer_handle_all() {
	timer_handle_all_of_group(TIMER_GROUP_ALL);
}
/***********************************************
*
* Description:
*   handle timer event which's group is the timer_group
*
************************************************/
void timer_handle_all_of_group(uint8_t timer_group)
{
    int32_t curr_time = ssz_tick_time_now();
    int32_t i;

    for (i=0; i<kTimerIDMax; i++)
    {
        //if timer is active and expire, clear the timer and call the handle
        if(g_timer_mgr[i].is_activated &&
			ssz_tick_time_is_after_or_eq(curr_time, g_timer_mgr[i].expire_time) &&
			(timer_group&g_timer_mgr[i].group)
            )
        {
            if (g_timer_mgr[i].is_repeat)
            {
                //set timer again if need repeat
				timer_start_periodic_every((TimerID)i, g_timer_mgr[i].timer_size);
            }
            else
            {
                //clear timer
                timer_stop((TimerID)i);
            }
            //call handle func
            if (g_timer_mgr[i].handle_func)
            {
                g_timer_mgr[i].handle_func();

                //after timer handle may be have post some msg, so need call msg handle at once
                //CtlEventHandleAll();
            }
        }
    }
}
