/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ssz_def.h"
#include "timer_config.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TimerHandler)();


//set handler of timer id, the timer group is in TIMER_GROUP_NORMAL
void timer_set_handler(TimerID id, TimerHandler func);
void timer_set_handler_and_group(TimerID id, TimerHandler func, uint8_t timer_group);

//start a timer(previous timer will be clear),
//when milliseconds from now is pass, the handle function will be called.
//Note: the timer only run once,
//if you want to run again, please call it again at handle function.
//the timer is base on tick, so suggest milliseconds is Multiples of one tick
void timer_start_oneshot_after(TimerID id, int32_t milliseconds);

//same as timer_start_oneshot_after, but it runs repeat,
//the handle function will be called each milliseconds.
//if you want to stop, please call timer_clear.
void timer_start_periodic_every(TimerID id, int32_t milliseconds);

//start a timer(previous timer will be clear),
//when tick_time is arrive, the handle function will be called.
//Note: the timer only run once,
void timer_start_oneshot_at(TimerID id, int32_t tick_time);

//stop the timer
void timer_stop(TimerID id);

//check if the timer active
bool timer_is_actived(TimerID id);

//stop all timer which's group is the timer_group
void timer_stop_all(uint8_t timer_group);

//stop and clear all timer handler which's group is the timer_group
void timer_stop_and_clear_all_handler(uint8_t timer_group);

//handle timer event
void timer_handle_all();

//handle timer event which's group is the timer_group
void timer_handle_all_of_group(uint8_t timer_group);




#ifdef __cplusplus
}
#endif



