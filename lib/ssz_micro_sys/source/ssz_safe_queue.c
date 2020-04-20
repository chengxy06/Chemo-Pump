#include "ssz_safe_queue.h"
#include "ssz_common.h"
/************************************************
* Declaration
************************************************/
#define M_get_node(container, index) ((uint8_t*)((container)->p_nodes + (index)*(container)->one_node_byte_size))
#define M_get_data_address(container,index) M_get_node(container, index)
#define M_get_data_size(container) ((container)->one_node_byte_size)
/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/


//init by the node array and node array size
void ssz_safe_queue_init(SszSafeQueue * container, void * p_nodes, int node_count, int one_node_byte_size){
	ssz_assert(container && p_nodes);
	container->max_size = (int16_t)node_count;
	container->start_index = 0;
	container->next_write_index = 0;
	container->one_node_byte_size = (int16_t)one_node_byte_size;
	container->p_nodes = (uint8_t*)p_nodes;
}

//Capacity:
//judge is empty
bool ssz_safe_queue_is_empty(SszSafeQueue* container){
	if (container->start_index == container->next_write_index) {
		return true;
	}
	else {
		return false;
	}
}
//judge is full
bool ssz_safe_queue_is_full(SszSafeQueue* container){
	int tmp_index = container->next_write_index + 1;
	if (tmp_index>=container->max_size) {
		tmp_index = 0;
	}
	if (tmp_index == container->start_index) {
		return true;
	}
	else {
		return false;
	}
}

int ssz_safe_queue_size(SszSafeQueue* container){
	int size = container->next_write_index - container->start_index;
	if (size<0) {
		size += container->max_size;
	}

	return size;
}


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_safe_queue_front(SszSafeQueue* container){
	ssz_assert(container);
	if (container->start_index != container->next_write_index) {
		return M_get_node(container, container->start_index);
	}
	else {
		return NULL;
	}
}
//return last element's address
//return:  null->not exist
void* ssz_safe_queue_back(SszSafeQueue* container){
	ssz_assert(container);
	if (container->start_index != container->next_write_index) {
		int index = container->next_write_index-1;
		if (index<0) {
			index += container->max_size;
		}
		return M_get_node(container, index);
	}
	else {
		return NULL;
	}
}

//Modifiers:
//delete first node
void ssz_safe_queue_pop(SszSafeQueue * container){
	if (container->start_index != container->next_write_index) {
		int index = container->start_index+1;
		if (index >= container->max_size) {
			index = 0;
		}
		container->start_index = index;
	}
}

//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_safe_queue_emplace(SszSafeQueue* container){
	int index = container->next_write_index + 1;
	if (index >= container->max_size) {
		index = 0;
	}
	if (index == container->start_index) {
		//it if full
		return NULL;
	}
	else
	{
		void* ret = M_get_node(container, container->next_write_index);
		container->next_write_index = index;
		return ret;
	}
}
//insert node at tail,set the node's data 
//return:  -1 -> fail because it is full, 0->success
int ssz_safe_queue_push(SszSafeQueue* container, const void* data){
	int index = container->next_write_index + 1;
	if (index >= container->max_size) {
		index = 0;
	}
	if (index == container->start_index) {
		//it if full
		return -1;
	}
	else
	{
		memcpy(M_get_data_address(container, container->next_write_index), data, M_get_data_size(container));
		container->next_write_index = index;
		return 0;
	}
}