#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
typedef struct{
    int16_t size;           /* size */
    int16_t max_size;       /* max size */
    uint8_t* p_nodes;   /* node array */
    int16_t one_node_byte_size;     /* size of one node */
}SszVector;

//return:  <0 when data1<data2, 0 when same, >0 when data1>data2
typedef int (*SszVectorCompareFun)(const void* data1, const void* data2, size_t data_size);

#ifdef __cplusplus
extern "C" {
#endif
	
//init by the node array and node array size
void ssz_vector_init(SszVector * container, void * p_nodes, int node_count, int one_node_byte_size);

//Iterators:
//return the first node's position
//if return -1, it is indicate that it is empty
int ssz_vector_begin(SszVector * container);
//return the next node's pos
//if return -1, it is indicate that it arrive the end
int ssz_vector_next( SszVector * container, int pos );
//return the previous node's position
//if return -1, it is indicate that it arrive the begin
int ssz_vector_prev(SszVector * container, int pos);
//return the last node's position
//if return -1, it is indicate that it is empty
int ssz_vector_rbegin(SszVector * container);


//Capacity:
//judge is empty
bool ssz_vector_is_empty(SszVector* container);
//judge is full
bool ssz_vector_is_full(SszVector* container);

int ssz_vector_size(SszVector* container);


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_vector_front(SszVector* container);
//return last element's address
//return:  null->not exist
void* ssz_vector_back(SszVector* container);
//return the element's address at the pos
//return:  null->not exist
void* ssz_vector_at_pos(SszVector* container, int pos);
//return the element's address at the index, index range is 0~size-1
void* ssz_vector_at_index(SszVector* container, int index);
int ssz_vector_pos_to_index(SszVector* container, int pos);
int ssz_vector_index_to_pos(SszVector* container, int index);

//Search:
//find the data's position, e.g.  ssz_vector_find(list, a, memcmp)
//return:  -1->not exist
int ssz_vector_find(SszVector * container, const void* data, SszVectorCompareFun compare_fun);
//find the data's position from the pos
//return:  -1->not exist
int ssz_vector_find_from(SszVector * container, int start_find_pos, const void* data, SszVectorCompareFun compare_fun);

//Modifiers:
//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_vector_emplace_back(SszVector* container);
//insert node at tail,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_vector_push_back(SszVector* container, const void* data);
//delete tail node
void ssz_vector_pop_back(SszVector * container);

//insert the node at the position, return the insert data
//return:  null -> fail because it is full
void* ssz_vector_emplace(SszVector * container, int pos);
//insert the node at the position, set the node's data and return the insert pos
//return:  -1 -> fail because it is full
int ssz_vector_insert(SszVector * container, int pos, const void* data);

//delete the data at the pos, and return the next element pos
//if the pos is the last, it will return -1 
int ssz_vector_erase(SszVector * container, int pos);

//clear all 
void ssz_vector_clear(SszVector * container);

//Operations:
//remove the nodes which's value is equal with the data
void ssz_vector_remove(SszVector * container, const void* compare_data, SszVectorCompareFun compare_fun);

//delete the continue same nodes
void ssz_vector_unique(SszVector * container, SszVectorCompareFun compare_fun);

//data_tmp is used to save the tmp data
void ssz_vector_swap(SszVector * container, int pos1, int pos2);

//sort by the fun, data_tmp is used to save the tmp data, 
//it use ascending
void ssz_vector_sort(SszVector * container, SszVectorCompareFun compare_fun);

//sort by the fun, data_tmp is used to save the tmp data
void ssz_vector_sort_by_descend(SszVector* container, SszVectorCompareFun compare_fun);

//reverse the container
void ssz_vector_reverse(SszVector * container);


#ifdef __cplusplus
}
#endif