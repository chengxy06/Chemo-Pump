#include "ssz_queue.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/


//init by the node array and node array size
void ssz_queue_init(SszQueue * container, void * p_nodes, int node_count, int one_node_byte_size){
	ssz_deque_init(container, p_nodes, node_count, one_node_byte_size);
}

//Capacity:
//judge is empty
bool ssz_queue_is_empty(SszQueue* container){
	return ssz_deque_is_empty(container);
}
//judge is full
bool ssz_queue_is_full(SszQueue* container){
	return ssz_deque_is_full(container);
}

int ssz_queue_size(SszQueue* container){
	return ssz_deque_size(container);
}


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_queue_front(SszQueue* container){
	return ssz_deque_front(container);
}
//return last element's address
//return:  null->not exist
void* ssz_queue_back(SszQueue* container){
	return ssz_deque_back(container);
}

//Modifiers:
//delete first node
void ssz_queue_pop(SszQueue * container){
	ssz_deque_pop_front(container);
}

//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_queue_emplace(SszQueue* container){
	return ssz_deque_emplace_back(container);
}
//insert node at tail,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_queue_push(SszQueue* container, const void* data){
	return ssz_deque_push_back(container, data);
}