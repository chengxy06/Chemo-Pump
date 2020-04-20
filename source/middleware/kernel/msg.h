/************************************************
* DESCRIPTION:
* 	!!!this is not thread safe, so do not use it at interrupt or other task
* 
************************************************/
#pragma once
#include "ssz_def.h"
#include "msg_config.h"
/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif



typedef union{
	int32_t int_param;
	const void* addr_param;
	struct {
		int16_t int16_param1;
		int16_t int16_param2;
	};
}MsgParam;
typedef struct 
{
	MsgParam param;
	MsgID msg_id;
}MsgOne;

typedef void (*MsgHandler)(MsgParam param);

void msg_init();

//get the first msg, if no msg, will return NULL
MsgOne* msg_font();

//delete first msg
void msg_pop();

//handle msg
void msg_handle(MsgOne *one_msg);

//handle all msgs until no msg
void msg_handle_all(void);

//post msg then return immediately
void msg_post_int(MsgID msg, int32_t param);
void msg_post_addr(MsgID msg, const void* param);
void msg_post_two_param(MsgID msg, int16_t param1, int16_t param2);

//notify the msg and wait until the msg is handled
//the msg will handle at once and not care the msg queue
void msg_notify_int_at_once(MsgID msg, int32_t param);
void msg_notify_addr_at_once(MsgID msg, const void* param);
void msg_notify_two_param_at_once(MsgID msg, int16_t param1, int16_t param2);

//set handler function for msg
void msg_set_handler(MsgID msg, MsgHandler func);

//clear all msg's handler
void msg_clear_all_handler(void);

//is not exist msg
bool msg_is_empty(void);

//is exist the msg which not handled
bool msg_is_exist(MsgID msg);

//is exist any msg
bool msg_is_exist_any(void);


#ifdef __cplusplus
}
#endif


