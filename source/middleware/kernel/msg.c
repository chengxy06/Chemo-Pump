/************************************************
* DESCRIPTION:
*   
************************************************/
#include "msg.h"
#include "ssz_queue.h"
#include "ssz_common.h"
#include "event.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static MsgOne g_msg_nodes[MSG_QUEUE_MAX_NUM];
static SszQueue g_msg_queue_inter;
static SszQueue *g_msg_queue=&g_msg_queue_inter;
static MsgHandler g_msg_handler_arr[kMsgIDMax];

/************************************************
* Function 
************************************************/

void msg_init(){
	ssz_queue_init(g_msg_queue, g_msg_nodes, ssz_array_size(g_msg_nodes), sizeof(g_msg_nodes[0]));
	event_set_handler(kEventReceiveNewMsg, msg_handle_all);
}
//get the first msg, if no msg, will return NULL
MsgOne* msg_font(){
	return (MsgOne*)ssz_queue_front(g_msg_queue);
}

//delete first msg
void msg_pop(){
	ssz_queue_pop(g_msg_queue);
}


//handle msg
void msg_handle(MsgOne *one_msg)
{
	ssz_assert(one_msg->msg_id<kMsgIDMax);
    if (g_msg_handler_arr[one_msg->msg_id])
    {
        g_msg_handler_arr[one_msg->msg_id](one_msg->param);
    }
}

//handle all msgs until no msg
void msg_handle_all(void)
{
    MsgOne *one_msg;

    while(!ssz_queue_is_empty(g_msg_queue)){
    	one_msg = (MsgOne*)ssz_queue_front(g_msg_queue);
    	msg_handle(one_msg);
    	ssz_queue_pop(g_msg_queue);
    }

}

static void msg_post(MsgID msg, MsgParam param){
	MsgOne *one_msg = (MsgOne*)ssz_queue_emplace(g_msg_queue);
	ssz_check(one_msg);
    if (one_msg) {
		one_msg->msg_id = msg;
		one_msg->param = param;
		event_set(kEventReceiveNewMsg);
	}else{
        
	}
}
//post msg then return immediately
void msg_post_int(MsgID msg, int32_t param){
	MsgParam msg_param;
	msg_param.int_param = param;
	msg_post(msg, msg_param);
}
void msg_post_addr(MsgID msg, const void* param){
	MsgParam msg_param;
	msg_param.addr_param = param;
	msg_post(msg, msg_param);
}
void msg_post_two_param(MsgID msg, int16_t param1, int16_t param2) {
	MsgParam msg_param;
	msg_param.int16_param1 = param1;
	msg_param.int16_param2 = param2;
	msg_post(msg, msg_param);
}
//notify the msg and wait until the msg is handled
//the msg will handle at once and not care the msg queue
void msg_notify_int_at_once(MsgID msg, int32_t param){
	MsgOne one_msg;
	one_msg.msg_id = msg;
	one_msg.param.int_param = param;
	msg_handle(&one_msg);
}
void msg_notify_addr_at_once(MsgID msg, const void* param){
	MsgOne one_msg;
	one_msg.msg_id = msg;
	one_msg.param.addr_param = param;
	msg_handle(&one_msg);
}
void msg_notify_two_param_at_once(MsgID msg, int16_t param1, int16_t param2) {
	MsgOne one_msg;
	one_msg.msg_id = msg;
	one_msg.param.int16_param1 = param1;
	one_msg.param.int16_param2 = param2;
	msg_handle(&one_msg);
}
//set handler function for msg
void msg_set_handler(MsgID msg, MsgHandler func){
	ssz_assert(msg<kMsgIDMax);
    g_msg_handler_arr[msg] = func;
}

//clear all msg's handler
void msg_clear_all_handler(void)
{
    int i;
    //clear all event handle
    for(i=0; i<ssz_array_size(g_msg_handler_arr); i++)
    {
        g_msg_handler_arr[i] = NULL;
    }
}

//is not exist msg
bool msg_is_empty(void)
{
    return ssz_queue_is_empty(g_msg_queue);
}

//return:  <0 when data1<data2, 0 when same, >0 when data1>data2
int msg_compare_by_msg_id(const void* data1, const void* data2, size_t data_size) {
	MsgOne *msg1 = (MsgOne*)data1;
	MsgOne *msg2 = (MsgOne*)data2;
	if (msg1->msg_id == msg2->msg_id) {
		return 0;
	}
	else {
		return -1;
	}
}
//is exist the msg which not handled
bool msg_is_exist(MsgID msg) {
	MsgOne one_msg;
	one_msg.msg_id = msg;
	if (ssz_deque_find(g_msg_queue, &one_msg, msg_compare_by_msg_id) != -1) {
		return true;
	}
	else {
		return false;
	}
}

//is exist any msg
bool msg_is_exist_any(void)
{
    return !msg_is_empty();
}

