/************************************************
* DESCRIPTION:
*   
************************************************/
#include "key.h"
#include "drv_key.h"
#include "timer.h"
#include "ssz_common.h"
#include "msg.h"
#include "config.h"

#if KEY_MODULE_ENABLE
/************************************************
* Declaration
************************************************/
//key info
typedef struct  
{
    KeyEventHandler handler[kKeyEventMax];
	int32_t hold_time;//unit: ms
	int16_t repeat_time;//unit:ms
	int16_t repeat_count;//
	int16_t hold_time_after_repeat_start;//unit:ms
	//is the key block when someone key is pressed
    bool is_block_when_one_key_pressed;
    int8_t stabilize_time;//the time to hold one value, unit:ms
	int8_t last_stabilize_value;
    KeyState last_state;
}KeyInfo;

//combination key info
typedef struct  
{
	KeyCombinationEventHandler handler[2];
	bool is_valid;
    KeyID key1;
    KeyID key2;
}CombinationKeyInfo;

/************************************************
* Variable 
************************************************/
//all key handlers
static KeyInfo g_key_infos[kKeyIDMax];
//combination key 
static CombinationKeyInfo g_combination_key_infos[KEY_COMBINATION_HANDLER_MAX];
//monitor funs
static KeyMonitorEventHandler g_key_monitor_funs[kKeyMonitorEventMax];
static KeyCombinationMonitorEventHandler g_combination_key_monitor_funs[kKeyMonitorEventMax];
//static KeyID g_key_current_handle_key;
static bool g_key_current_key_is_handled;
static bool g_key_current_key_is_need_handle;
static int32_t g_key_last_read_time;

/************************************************
* Function 
************************************************/

static void key_scan_timer_out() {
	key_scan(ssz_tick_time_elapsed(g_key_last_read_time));
	g_key_last_read_time = ssz_tick_time_now();
}
/***********************************************
* Description:
*   key init, clear all key info, start timer to scan
* Argument:
*
* Return:
*
************************************************/
void key_init(void)
{   
	ssz_mem_zero(g_key_infos, sizeof(g_key_infos));
	ssz_mem_zero(g_combination_key_infos, sizeof(g_combination_key_infos));
	ssz_mem_zero(g_key_monitor_funs, sizeof(g_key_monitor_funs));

    timer_set_handler(kTimerScanKey, key_scan_timer_out);
}

//start scan key
void key_start_scan(){
	g_key_last_read_time = ssz_tick_time_now();
	for (int i = 0; i < kKeyIDMax; i++)
	{
		g_key_infos[i].stabilize_time = 0;
	}
	timer_start_periodic_every(kTimerScanKey,KEY_SCAN_TIME_MS);
	key_scan(KEY_SCAN_TIME_MS);
}

void key_stop_scan()
{
	timer_stop(kTimerScanKey);
}

//it will return -1 if not find the key
//if find the key, do not use first_invalid_node_index
//if not find any invalid node, the first_invalid_node_index will return -1
int key_find_combination_index(KeyID key1, KeyID key2, int* first_invalid_node_index) {
	int invalid_node_index = -1;
	for (int i = 0; i < ssz_array_size(g_combination_key_infos); i++) {
		if (g_combination_key_infos[i].is_valid &&
			((g_combination_key_infos[i].key1 == key1 && g_combination_key_infos[i].key2 == key2) ||
			(g_combination_key_infos[i].key1 == key2 && g_combination_key_infos[i].key2 == key1))
			) {
			return i;
		}
		else if (!g_combination_key_infos[i].is_valid && invalid_node_index==-1) {
			invalid_node_index = i;
		}
	}

	if (first_invalid_node_index) {
		*first_invalid_node_index = invalid_node_index;
	}

	return -1;
}

/***********************************************
* Description:
*   used to let the key press is OK if the key's other event is exist, then can play bip
* Argument:
*
* Return:
*
************************************************/
//static void key_on_press(void)
//{
//    int i;
//    //if the key's other event is exist, return true
//    for(i=kKeyEventReleased; i<kKeyEventMax; i++)
//    {
//        if (g_key_infos[g_key_current_handle_key].handler[i] != NULL)
//        {
//            key_set_event_handle_state(true);
//        }
//    }
//
//    //if the combination key's other event is exist, return true
//    for(i=0; i<KEY_COMBINATION_HANDLER_MAX; i++)
//    {
//    	if(!g_combination_key_infos[i].is_valid){
//        		continue;
//        }
//        if (g_combination_key_infos[i].handler[kKeyEventPressed] != NULL
//            ||g_combination_key_infos[i].handler[kKeyEventReleased]!=NULL)
//        {
//            if(g_combination_key_infos[i].key1==g_key_current_handle_key
//                ||g_combination_key_infos[i].key2==g_key_current_handle_key)
//            {
//				key_set_event_handle_state(true);
//            }
//        } 
//    }
//
//}

//bind handle function for key event, it will clear repeat time
void key_set_handler(KeyID key, KeyEvent event, KeyEventHandler fun)
{
    ssz_assert(key<kKeyIDMax && event<kKeyEventMax);
    g_key_infos[key].handler[event] = fun;
	key_set_key_repeat_speed_by_time(key, 0);
    //if bind valid fun on event, need set pressed event handle not null
    //if (fun != NULL && event!=kKeyEventPressed)
    //{
    //    if(g_key_infos[key].handler[kKeyEventPressed]==NULL)
    //    {
    //        g_key_infos[key].handler[kKeyEventPressed] = key_on_press;
    //    }
    //}
}
//bind handle function for key event, it will set repeat time as KEY_REPEAT_TIME_MS_AFTER_HOLD
void key_set_handler_with_repeat(KeyID key, KeyEvent event, KeyEventHandler fun) {
	ssz_assert(key < kKeyIDMax && event < kKeyEventMax);
	g_key_infos[key].handler[event] = fun;
	key_set_key_repeat_speed_by_time(key, KEY_REPEAT_TIME_MS_AFTER_HOLD);
}
//bind handle function for combination key event,
//Note: it only support pressed and released event, not support repeat
void key_set_combination_handler(KeyID key1, KeyID key2, KeyEvent event, KeyCombinationEventHandler fun)
{
    ssz_assert(key1<kKeyIDMax &&key2<kKeyIDMax &&key1!=key2 && event<kKeyEventMax);
    ssz_assert(event ==kKeyEventPressed || event==kKeyEventReleased);
	
	int empty_node_index = -1;
	int index = key_find_combination_index(key1, key2, &empty_node_index);

    if (fun == NULL)
	{
		//clear the combination key
		if (index != -1) {
			ssz_mem_zero(&g_combination_key_infos[index], sizeof(g_combination_key_infos[index]));
		}
	}else
    {
		if (index == -1) {
			index = empty_node_index;
		}
		ssz_assert(index != -1);
		if (index != -1) {
			//add or change the info
			g_combination_key_infos[index].key1 = key1;
			g_combination_key_infos[index].key2 = key2;
			g_combination_key_infos[index].handler[event] = fun;
			g_combination_key_infos[index].is_valid = true;

			//if bind valid fun on event, need set the two key's pressed event handle not null
			//if (g_key_infos[key1].handler[kKeyEventPressed] == NULL)
			//{
			//	g_key_infos[key1].handler[kKeyEventPressed] = key_on_press;
			//}

			//if (g_key_infos[key2].handler[kKeyEventPressed] == NULL)
			//{
			//	g_key_infos[key2].handler[kKeyEventPressed] = key_on_press;
			//}
		}
    }
}

//bind handle function to monitor key
void key_set_monitor_event_handler(KeyMonitorEvent event, KeyMonitorEventHandler fun)
{
	ssz_assert(event < kKeyMonitorEventMax);
	g_key_monitor_funs[event] = fun;
}
void key_set_combination_monitor_event_handler(KeyMonitorEvent event, KeyCombinationMonitorEventHandler fun) {
	ssz_assert(event < kKeyMonitorEventMax);
	g_combination_key_monitor_funs[event] = fun;
}

//clear all key handle(include combination key)
void key_clear_all_handler(void)
{
	for (int i = 0; i < ssz_array_size(g_key_infos); ++i) {
		memset(g_key_infos[i].handler, 0, sizeof(g_key_infos[0].handler));
		key_set_key_repeat_speed_by_time((KeyID)i, 0);
	}
    
    memset(g_combination_key_infos, 0, sizeof(g_combination_key_infos));
}

//clear combination key handle
void key_clear_all_combination_handler(void)
{
    memset(g_combination_key_infos, 0, sizeof(g_combination_key_infos));
}

//clear one key's handle
void key_clear_handler(KeyID key)
{
    memset(g_key_infos[key].handler, 0, sizeof(g_key_infos[key].handler));
	key_set_key_repeat_speed_by_time(key, 0);
}


/***********************************************
* Description:
*   set all key blocked when other key is pressed
* Argument:
*
* Return:
*
************************************************/
void key_set_all_blocked_when_key_pressed(void)
{
	int i;

	for (i = 0; i < kKeyIDMax; i++)
	{
		g_key_infos[i].is_block_when_one_key_pressed = true;
	}
}

/***********************************************
* Description:
*   set the key block or not when other key is pressed
* Argument:
*   key:
*   is_blocked:
*
* Return:
*
************************************************/
void key_set_block_state_when_key_pressed(KeyID key, bool is_blocked)
{
	g_key_infos[key].is_block_when_one_key_pressed = is_blocked;
}

//set key press repeat speed(time) after key hold
//when after hold, the key press handler will call each the repeat speed
// time_ms_repeat_speed: if <=0, will not repeat
void key_set_key_repeat_speed_by_time(KeyID key, int time_ms_repeat_speed)
{
	g_key_infos[key].repeat_time = time_ms_repeat_speed;
	g_key_infos[key].hold_time_after_repeat_start = 0;
	g_key_infos[key].repeat_count = 0;
}

void key_clear_all_repeat_speed()
{
	for (int i = 0; i < kKeyIDMax; i++)
	{
		g_key_infos[i].repeat_time = 0;
		g_key_infos[i].hold_time_after_repeat_start = 0;
		g_key_infos[i].repeat_count = 0;
	}
}

/***********************************************
* Description:
*   is the key pressed
* Argument:
*   key:
*
* Return:
*
************************************************/
bool key_is_pressed(KeyID key)
{
  bool is_pressed = false;
 
  ssz_assert(key<kKeyIDMax);
  
  
  if(key_state(key)>=kKeyStatePress)
  {
    is_pressed = true;
  }
    
  return is_pressed;
}

/***********************************************
* Description:
*   is any key pressed except one key
* Argument:
*   key:
*
* Return:
*
************************************************/
bool key_is_any_key_pressed_except( KeyID key )
{
    int i;
    for (i=0; i<kKeyIDMax; i++)
    {
        //check if key is pressed
        if (i!=key && key_is_pressed((KeyID)i))
        {
            return true;
        }
    }
    return false;
}

//get key state
KeyState key_state(KeyID key)
{
	KeyState state;
	int32_t hold_time = g_key_infos[key].hold_time;


	if (hold_time >= KEY_HOLD_TIME_MS)
	{
		state = kKeyStateHold;
	}
	else if (hold_time > 0)
	{
		state = kKeyStatePress;
	}
	else {
		state = kKeyStateNone;
	}
	return state;
}

/***********************************************
* Description:
*   get the key's hold time
* Argument:
*   key:
*
* Return:
*
************************************************/
int32_t key_hold_time(KeyID key)
{
    return g_key_infos[key].hold_time;
}

void key_set_hold_time(KeyID key, int32_t hold_time_ms){
	g_key_infos[key].hold_time = hold_time_ms;	
	g_key_infos[key].last_state = key_state(key);
	g_key_infos[key].stabilize_time = 0;
	if(g_key_infos[key].last_state==kKeyStateNone)
	{
		g_key_infos[key].last_stabilize_value = 0;
	}else{
		g_key_infos[key].last_stabilize_value = 1;
	}
}



/***********************************************
* Description:
*   get how many key is pressed
* Argument:
*
* Return:
*
************************************************/
static int key_pressed_counter(void)
{
    int pressed_key_total = 0;
    int i;

    //calc how many keys is pressed
    for(i=0; i<kKeyIDMax; i++)
    {
        if(key_is_pressed((KeyID)i))
        {
            pressed_key_total++;
        }
    }

    return pressed_key_total;
}

/***********************************************
* Description:
*   handle key event
* Argument:
*   key:
*   event:
*   combination_key_handle_valid: 
*       TRUE: it is combination key and the combination key has valid handle function
*       FALS: it is not combination key or the combination key has unvalid handle function 
*
* Return:
*
************************************************/
static void key_handle(KeyID key, KeyEvent msg, bool combination_key_handle_valid, int repeat_count)
{
    KeyEventHandler msg_fun;
    
    ssz_assert(key<kKeyIDMax && msg<kKeyEventMax);

    //g_key_current_handle_key = key;
	g_key_current_key_is_handled = false;
	g_key_current_key_is_need_handle = true;
    //call monitor fun
	if (g_key_monitor_funs[kKeyMonitorEventBefore]) {
		g_key_monitor_funs[kKeyMonitorEventBefore](key, msg, false, repeat_count);
	}

    msg_fun = g_key_infos[key].handler[msg];

	if (!g_key_current_key_is_need_handle) {
		msg_fun = NULL;
	}
    //if the event is not release, check if it can handle
    if(msg_fun!=NULL && msg!=kKeyEventReleased)
    {
        //check if it is combination key and the combination key has valid handle function
        //if yes, no need handle the event and if the handle function is exist, it need set handle result as TRUE to play a beep
        if(combination_key_handle_valid)
        {
			g_key_current_key_is_handled = true;
            msg_fun = NULL; 
        }
        //check if this is blocked when exist valid pressed key
        else if(g_key_infos[key].is_block_when_one_key_pressed && key_pressed_counter()>1)
        {
            //if this key is blocked, do not call the handler
            msg_fun = NULL;
        }
    }
    
    //handle the key
    if(msg_fun != NULL)
    {
		g_key_current_key_is_handled = true;
        msg_fun(key, repeat_count);
    }

    //call monitor fun
	if (g_key_monitor_funs[kKeyMonitorEventAfter]) {
		g_key_monitor_funs[kKeyMonitorEventAfter](key, msg, g_key_current_key_is_handled, repeat_count);
	}
}

/***********************************************
* Description:
*   handle combination key event
* Argument:
*   combination_key:
*   event:
*
* Return:
*
************************************************/
static void key_combination_handle(int combination_key_index, KeyEvent msg)
{
	KeyCombinationEventHandler msg_fun;
    
    ssz_assert(combination_key_index>=0 && 
		combination_key_index<KEY_COMBINATION_HANDLER_MAX && msg<kKeyEventHold);

	g_key_current_key_is_handled = false;
	g_key_current_key_is_need_handle = true;
	//call monitor fun
	if (g_combination_key_monitor_funs[kKeyMonitorEventBefore]) {
		g_combination_key_monitor_funs[kKeyMonitorEventBefore](
			g_combination_key_infos[combination_key_index].key1,
			g_combination_key_infos[combination_key_index].key2,
			msg, false);
	}

    msg_fun = g_combination_key_infos[combination_key_index].handler[msg];
   
	if (!g_key_current_key_is_need_handle) {
		msg_fun = NULL;
	}

    if(msg_fun != NULL)
    {
        msg_fun(g_combination_key_infos[combination_key_index].key1,
			g_combination_key_infos[combination_key_index].key2);
    }

	//call monitor fun
	if (g_combination_key_monitor_funs[kKeyMonitorEventAfter]) {
		g_combination_key_monitor_funs[kKeyMonitorEventAfter](
			g_combination_key_infos[combination_key_index].key1,
			g_combination_key_infos[combination_key_index].key2,
			msg, g_key_current_key_is_handled);
	}
}

/***********************************************
* Description:
*   handle key press
* Argument:
*   param:
*
* Return:
*
************************************************/
static void key_press_handle(int param, int repeat_count)
{
    int i = 0;
    bool is_combination_pressed = false;
    int pressed_key_counter  = key_pressed_counter();

    //if press just two keys, check if it is combination key
    if(pressed_key_counter == 2)
    {
        //check if combination key is pressed
        for (i=0; i<ssz_array_size(g_combination_key_infos); i++)
        {
        	if(!g_combination_key_infos[i].is_valid){
        		continue;
        	}
            //check if two key is pressed 
            if((param == g_combination_key_infos[i].key1) && key_is_pressed(g_combination_key_infos[i].key2))
            {
                //another key is already pressed,so combination key is pressed
                is_combination_pressed = true;
            }
            else if((param == g_combination_key_infos[i].key2) && key_is_pressed(g_combination_key_infos[i].key1))
            {
                //another key is already pressed,so combination key is pressed
                is_combination_pressed = true;
            }
            if (is_combination_pressed)
            {
                break;
            }
        }
    }

    if(is_combination_pressed)ssz_assert(i<ssz_array_size(g_combination_key_infos));
    //if it is combination key and it has bind fun, handle the key with null event fun
    if(is_combination_pressed && 
        (g_combination_key_infos[i].handler[kKeyEventPressed]
         ||g_combination_key_infos[i].handler[kKeyEventReleased])
        )
    {
		if (repeat_count > 0) {
			return;
		}
        key_handle((KeyID)param, kKeyEventPressed, true, 0);
        key_combination_handle(i, kKeyEventPressed);
        return;
    }
    else
    {
        key_handle((KeyID)param, kKeyEventPressed, false, repeat_count);
    }

}

/***********************************************
* Description:
*   handle key release
* Argument:
*   param:
*
* Return:
*
************************************************/
static void key_release_handle(int param)
{
    int i;

    key_handle((KeyID)param, kKeyEventReleased, false, 0);

    //check if combination key is released
    for (i=0; i<ssz_array_size(g_combination_key_infos); i++)
    {
    	if(!g_combination_key_infos[i].is_valid){
        		continue;
        }
        //check if key is one of combination key
        if( (param == g_combination_key_infos[i].key1 && key_is_pressed(g_combination_key_infos[i].key2))
            || (param == g_combination_key_infos[i].key2  && key_is_pressed(g_combination_key_infos[i].key1))
           )
        {
            //it is, so combination key is released
            key_combination_handle(i, kKeyEventReleased);
        }
    }
}


void key_set_event_handle_state(bool is_key_handled)
{
	g_key_current_key_is_handled = is_key_handled;
}

//used in before key press event handler, if you want the key skip handle, call it
void key_disable_handle_once() {
	g_key_current_key_is_need_handle = false;
}
//elapsed_time_ms: the time elapse from last call
void key_scan(int elapsed_time_ms)
{
	KeyState key_curr_state;

	int key;
	int8_t key_states[kKeyIDMax];

	drv_key_scan(key_states);

	for (key = 0; key < kKeyIDMax; key++)
	{
		if (key_states[key] != g_key_infos[key].last_stabilize_value) {
			//if the value changed, check if it can stabilize
			g_key_infos[key].stabilize_time += elapsed_time_ms;
			if (g_key_infos[key].stabilize_time>= KEY_STABILIZE_TIME_MS) {
				//if the value is stabilize, use the value
				g_key_infos[key].last_stabilize_value = key_states[key];
				g_key_infos[key].stabilize_time = 0;
			}
		}
		else {
			g_key_infos[key].stabilize_time = 0;
		}

		//calc the hold time
		if (g_key_infos[key].last_stabilize_value == 1) {
			if (g_key_infos[key].hold_time == 0) {
				g_key_infos[key].hold_time = KEY_STABILIZE_TIME_MS;
			}
			else {
				g_key_infos[key].hold_time += elapsed_time_ms;
				if (g_key_infos[key].hold_time<0) {
					g_key_infos[key].hold_time = INT32_MAX;
				}
			}
		}
		else {
			g_key_infos[key].hold_time = 0;
		}
		key_curr_state = key_state((KeyID)key);

		if (key_curr_state != g_key_infos[key].last_state)
		{
			g_key_infos[key].last_state = key_curr_state;
			switch (key_curr_state)
			{
			case kKeyStateNone:
				g_key_infos[key].hold_time_after_repeat_start = 0;
				g_key_infos[key].repeat_count = 0;
				key_release_handle(key);
				break;
			case kKeyStatePress:
				g_key_infos[key].hold_time_after_repeat_start = 0;
				g_key_infos[key].repeat_count = 0;
				key_press_handle(key, 0);
				break;
			case kKeyStateHold:
				g_key_infos[key].hold_time_after_repeat_start = 0;
				g_key_infos[key].repeat_count = 0;
				key_handle((KeyID)key, kKeyEventHold, false, 0);
				break;
			default:
				ssz_assert_fail();
				break;
			}
		}

		//check repeat time
		if (g_key_infos[key].last_state >= kKeyStateHold && g_key_infos[key].repeat_time > 0) {
			g_key_infos[key].hold_time_after_repeat_start += elapsed_time_ms;
			if (g_key_infos[key].hold_time_after_repeat_start >= g_key_infos[key].repeat_time) {
				g_key_infos[key].hold_time_after_repeat_start = 0;
				g_key_infos[key].repeat_count++;
				key_press_handle(key, g_key_infos[key].repeat_count);
			}
		}

	}
}
#endif