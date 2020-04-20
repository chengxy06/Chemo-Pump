#pragma once
#include "ssz_def.h"
#include "ssz_deque.h"
/************************************************
* Declaration
************************************************/
typedef SszDeque SszQueue;

#ifdef __cplusplus
extern "C" {
#endif
	
//init by the node array and node array size
void ssz_queue_init(SszQueue * container, void * p_nodes, int node_count, int one_node_byte_size);

//Capacity:
//judge is empty
bool ssz_queue_is_empty(SszQueue* container);
//judge is full
bool ssz_queue_is_full(SszQueue* container);

int ssz_queue_size(SszQueue* container);


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_queue_front(SszQueue* container);
//return last element's address
//return:  null->not exist
void* ssz_queue_back(SszQueue* container);

//Modifiers：
//delete first node
void ssz_queue_pop(SszQueue * container);

//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_queue_emplace(SszQueue* container);
//insert node at tail,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_queue_push(SszQueue* container, const void* data);


#ifdef __cplusplus
}
#endif