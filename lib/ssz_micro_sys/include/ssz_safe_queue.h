#pragma once
#include "ssz_def.h"
#include "ssz_deque.h"
/************************************************
* Declaration
************************************************/
typedef struct {
	int16_t max_size;  		/* max size */
	int16_t start_index; 	/* start node index in node array*/
	int16_t next_write_index; 	/* the node index to write data in node array*/
	uint8_t* p_nodes; 	/* node array */
	int16_t one_node_byte_size; 	/* size of one node */
}SszSafeQueue;

#ifdef __cplusplus
extern "C" {
#endif


//init by the node array and node array size, the valid size=max_size-1
void ssz_safe_queue_init(SszSafeQueue * container, void * p_nodes, int node_count, int one_node_byte_size);

//Capacity:
//judge is empty
bool ssz_safe_queue_is_empty(SszSafeQueue* container);
//judge is full
bool ssz_safe_queue_is_full(SszSafeQueue* container);

int ssz_safe_queue_size(SszSafeQueue* container);


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_safe_queue_front(SszSafeQueue* container);
//return last element's address
//return:  null->not exist
void* ssz_safe_queue_back(SszSafeQueue* container);

//Modifiers：
//delete first node
void ssz_safe_queue_pop(SszSafeQueue * container);

//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_safe_queue_emplace(SszSafeQueue* container);
//insert node at tail,set the node's data  
//return:  -1 -> fail because it is full, 0->success
int ssz_safe_queue_push(SszSafeQueue* container, const void* data);


#ifdef __cplusplus
}
#endif