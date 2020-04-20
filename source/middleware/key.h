/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ssz_def.h"
#include "key_config.h"



/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define KEY_SCAN_TIME_MS 5
#define KEY_STABILIZE_TIME_MS 20
#define KEY_HOLD_TIME_MS 500
#define KEY_REPEAT_TIME_MS_AFTER_HOLD 200
#define KEY_COMBINATION_HANDLER_MAX 4

//all key event 
typedef enum
{
    kKeyEventPressed=0,
    kKeyEventReleased=1,
    kKeyEventHold,  
    kKeyEventMax
}KeyEvent;

//all monitor event
typedef enum
{
	kKeyMonitorEventBefore,
	kKeyMonitorEventAfter,
    kKeyMonitorEventMax,
}KeyMonitorEvent;


typedef enum
{
   kKeyStateNone, //no key is pressed
   kKeyStatePress,
   kKeyStateHold, //press continue time greater than KEY_HOLD_TIME_MS second
}KeyState;

//key handle function, default think it handled if it is call,
//if you want it not handled, call key_set_event_handle_state at the handler
//repeat_count: only for press event, when after hold, the press event will send each repeat time, 
//			this is the repeat count, 0:first press, 1: repeat once after hold, 2: repeat twice after hold...
typedef void(*KeyEventHandler)(KeyID key, int repeat_count);
typedef void(*KeyCombinationEventHandler)(KeyID key1, KeyID key2);
typedef void(*KeyMonitorEventHandler)(KeyID key, KeyEvent key_event, bool is_key_handled, int repeat_count);
typedef void(*KeyCombinationMonitorEventHandler)(KeyID key1, KeyID key2, KeyEvent key_event, bool is_key_handled);

//key init, clear all key info, start timer to scan
void key_init();

//start scan key
void key_start_scan();
void key_stop_scan();

//bind handle function for key event, it will clear repeat time
void key_set_handler(KeyID key, KeyEvent event, KeyEventHandler fun);
//bind handle function for key event, it will set repeat time as KEY_REPEAT_TIME_MS_AFTER_HOLD
void key_set_handler_with_repeat(KeyID key, KeyEvent event, KeyEventHandler fun);

//bind handle function for combination key event,
//Note: it only support pressed and released event, not support repeat
void key_set_combination_handler(KeyID key1, KeyID key2, KeyEvent event, KeyCombinationEventHandler fun);

//bind handle function to monitor key
void key_set_monitor_event_handler(KeyMonitorEvent event, KeyMonitorEventHandler fun);
void key_set_combination_monitor_event_handler(KeyMonitorEvent event, KeyCombinationMonitorEventHandler fun);

//clear all key handle(include combination key)
void key_clear_all_handler(void);

//clear combination key handle
void key_clear_all_combination_handler(void);

//clear one key's handle
void key_clear_handler(KeyID key);

//set all key blocked when other key is pressed
void key_set_all_blocked_when_key_pressed(void);

//set the key block or not when other key is pressed
void key_set_block_state_when_key_pressed(KeyID key, bool is_blocked);

//set key press repeat speed(time) after key hold
//when after hold, the key press handler will call each the repeat speed
// time_ms_repeat_speed: if <=0, will not repeat
void key_set_key_repeat_speed_by_time(KeyID key, int time_ms_repeat_speed);

//clear all key's repeat speed
void key_clear_all_repeat_speed();

//is key pressed
bool key_is_pressed(KeyID key);

//is any key pressed except one key(you can use kKeyIDMax to check any key)
bool key_is_any_key_pressed_except(KeyID key);

//get key state
KeyState key_state(KeyID key);

//get the key's hold time
int32_t key_hold_time(KeyID key);

//you can set power key as hold when power up, 
//it can avoid receive power key pressed event after post
void key_set_hold_time(KeyID key, int32_t hold_time_ms);

//used in key event handler, if you want the key is not handled, call it
void key_set_event_handle_state(bool is_key_handled);

//used in before key press event handler, if you want the key skip handle, call it
void key_disable_handle_once();

//elapsed_time_ms: the time elapse from last call
void key_scan(int elapsed_time_ms);

#ifdef __cplusplus
}
#endif


