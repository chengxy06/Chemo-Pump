#include "ssz_stack.h"

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
void ssz_stack_init(SszStack * container, void * p_nodes, int node_count, int one_node_byte_size){
	ssz_deque_init(container, p_nodes, node_count, one_node_byte_size);
}

//Capacity:
//judge is empty
bool ssz_stack_is_empty(SszStack* container){
	return ssz_deque_is_empty(container);
}
//judge is full
bool ssz_stack_is_full(SszStack* container){
	return ssz_deque_is_full(container);
}

int ssz_stack_size(SszStack* container){
	return ssz_deque_size(container);
}


//Element access:
//return top element's address, 
//return:  null->not exist 
void* ssz_stack_top(SszStack* container){
	return ssz_deque_back(container);
}


//Modifiers:
//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_stack_emplace(SszStack* container){
	return ssz_deque_emplace_back(container);
}
//insert node at tail,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_stack_push(SszStack* container, const void* data){
	return ssz_deque_push_back(container, data);
}
//delete tail node
void ssz_stack_pop(SszStack * container){
	ssz_deque_pop_back(container);
}