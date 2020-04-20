#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
typedef struct{
    int16_t size;   		/* size */
    int16_t max_size;  		/* max size */
    int16_t start_index; 	/* start node index in node array*/
	uint8_t* p_nodes; 	/* node array */
    int16_t one_node_byte_size; 	/* size of one node */
}SszDeque;

#ifdef __cplusplus
extern "C" {
#endif
	
//return:  <0 when data1<data2, 0 when same, >0 when data1>data2
typedef int (*SszDequeCompareFun)(const void* data1, const void* data2, size_t data_size);

//init by the node array and node array size
void ssz_deque_init(SszDeque * container, void * p_nodes, int node_count, int one_node_byte_size);

//Iterators:
//return the first node's position
//if return -1, it is indicate that it is empty
int ssz_deque_begin(SszDeque * container);
//return the next node's pos
//if return -1, it is indicate that it arrive the end
int ssz_deque_next( SszDeque * container, int pos );
//return the previous node's position
//if return -1, it is indicate that it arrive the begin
int ssz_deque_prev(SszDeque * container, int pos);
//return the last node's position
//if return -1, it is indicate that it is empty
int ssz_deque_rbegin(SszDeque * container);


//Capacity:
//judge is empty
bool ssz_deque_is_empty(SszDeque* container);
//judge is full
bool ssz_deque_is_full(SszDeque* container);

int ssz_deque_size(SszDeque* container);


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_deque_front(SszDeque* container);
//return last element's address
//return:  null->not exist
void* ssz_deque_back(SszDeque* container);
//return the element's address at the pos
//return:  null->not exist
void* ssz_deque_at_pos(SszDeque* container, int pos);
//return the element's address at the index, index range is 0~size-1
void* ssz_deque_at_index(SszDeque* container, int index);
int ssz_deque_pos_to_index(SszDeque* container, int pos);
int ssz_deque_index_to_pos(SszDeque* container, int index);

//Search:
//find the data's position, e.g.  ssz_deque_find(list, a, memcmp)
//return:  -1->not exist
int ssz_deque_find(SszDeque * container, const void* data, SszDequeCompareFun compare_fun);
//find the data's position from the pos
//return:  -1->not exist
int ssz_deque_find_from(SszDeque * container, int start_find_pos, const void* data, SszDequeCompareFun compare_fun);

//Modifiers：
//insert node at head and return the inserted node data
//return:  null -> fail because it is full
void* ssz_deque_emplace_front(SszDeque* container);
//insert node at head,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_deque_push_front(SszDeque* container, const void* data);
//delete first node
void ssz_deque_pop_front(SszDeque * container);

//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_deque_emplace_back(SszDeque* container);
//insert node at tail,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_deque_push_back(SszDeque* container, const void* data);
//delete tail node
void ssz_deque_pop_back(SszDeque * container);

//insert the node at the position, return the insert data
//return:  null -> fail because it is full
void* ssz_deque_emplace(SszDeque * container, int pos);
//insert the node at the position, set the node's data and return the insert pos
//return:  -1 -> fail because it is full
int ssz_deque_insert(SszDeque * container, int pos, const void* data);

//delete the data at the pos, and return the next element pos
//if the pos is the last, it will return -1 
int ssz_deque_erase(SszDeque * container, int pos);

//clear all 
void ssz_deque_clear(SszDeque * container);

//Operations:
//remove the nodes which's value is equal with the data
void ssz_deque_remove(SszDeque * container, const void* compare_data, SszDequeCompareFun compare_fun);

//delete the continue same nodes
void ssz_deque_unique(SszDeque * container, SszDequeCompareFun compare_fun);

//data_tmp is used to save the tmp data
void ssz_deque_swap(SszDeque * container, int pos1, int pos2);

//sort by the fun, data_tmp is used to save the tmp data, 
//it use ascending
void ssz_deque_sort(SszDeque * container, SszDequeCompareFun compare_fun);

//sort by the fun, data_tmp is used to save the tmp data
void ssz_deque_sort_by_descend(SszDeque * container, SszDequeCompareFun compare_fun);

//reverse the container
void ssz_deque_reverse(SszDeque * container);


#ifdef __cplusplus
}
#endif