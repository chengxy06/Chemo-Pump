/************************************************
* DESCRIPTION:
*   
************************************************/
#include "event.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static EventHandler g_event_handlers[kEventIDMax];
int g_events[kEventIDMax];
/************************************************
* Function 
************************************************/

void event_set(EventID event) {
	ssz_assert(event < ssz_array_size(g_events));
	g_events[event] = 1;
}
void event_clear(EventID event) {
	ssz_assert(event < ssz_array_size(g_events));
	g_events[event] = 0;
}
bool event_is_set(EventID event) {
	ssz_assert(event < ssz_array_size(g_events));
	return g_events[event] == 1 ? true : false;
}

void event_set_handler(EventID event, EventHandler event_handler) {
	ssz_assert(event < kEventIDMax);
	g_event_handlers[event] = event_handler;
}
void event_handle(EventID event) {
	ssz_assert(event < kEventIDMax);
	if(g_event_handlers[event]){
		g_event_handlers[event]();
	}
}
