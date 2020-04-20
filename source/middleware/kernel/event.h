/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "event_config.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


typedef void(*EventHandler)();
extern int g_events[kEventIDMax];

void event_set(EventID event);
#define event_set_quick(event) (g_events[(event)] = 1)
void event_clear(EventID event);
bool event_is_set(EventID event);
void event_set_handler(EventID event, EventHandler event_handler);
void event_handle(EventID event);




#ifdef __cplusplus
}
#endif


