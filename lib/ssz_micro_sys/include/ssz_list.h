#pragma once
#include "ssz_def.h"
/************************************************
* Declaration
************************************************/
//if the index all is -1, mean this node is invalid
#define SSZ_LIST_NODE_BASE_DEFINE   int16_t prev_index;int16_t next_index
/* node, user should redefine the node*/
typedef struct{
    SSZ_LIST_NODE_BASE_DEFINE;
	//user should define the real data at below line
    //default it have a int data, user can redefine it
	union  {
		int user_data;
		void* user_data_addr;
	};
}SszListNode;

typedef struct{
    int16_t size;   		/* size */
    int16_t max_size;  		/* max size */
    int16_t head_index; 	/* first node index in node array, -1 is mean empty */
    int16_t tail_index; 	/* tail node index in node array, -1 is mean empty */
    uint8_t* p_nodes; 		/* node array */
    int16_t one_node_byte_size; 	/* size of one node */
}SszList;

#ifdef __cplusplus
extern "C" {
#endif
    
//return:  <0 when data1<data2, 0 when same, >0 when data1>data2
typedef int (*SszListCompareFun)(const void* data1, const void* data2, size_t data_size);

//init by the node array and node array size
void ssz_list_init(SszList * container, SszListNode * p_nodes, int node_count, int one_node_byte_size);

//Iterators:
//return the first node's position
//if return -1, it is indicate that it is empty
int ssz_list_begin(SszList * container);
//return the next node's pos
//if return -1, it is indicate that it arrive the end
int ssz_list_next( SszList * container, int pos );
//return the previous node's position
//if return -1, it is indicate that it arrive the begin
int ssz_list_prev(SszList * container, int pos);
//return the last node's position
//if return -1, it is indicate that it is empty
int ssz_list_rbegin(SszList * container);


//Capacity:
//judge is empty
bool ssz_list_is_empty(SszList* container);
//judge is full
bool ssz_list_is_full(SszList* container);

int ssz_list_size(SszList* container);


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_list_front(SszList* container);
//return last element's address
//return:  null->not exist
void* ssz_list_back(SszList* container);
//return the element's address at the pos
void* ssz_list_at_pos(SszList* container, int pos);

//Search:
//find the data's position, e.g.  ssz_list_find(list, a, memcmp)
//return:  -1->not exist
int ssz_list_find(SszList * container, const void* data, SszListCompareFun compare_fun);
//find the data's position from the pos
//return:  -1->not exist
int ssz_list_find_from(SszList * container, int start_find_pos, const void* data, SszListCompareFun compare_fun);

//Modifiers：
//insert node at head and return the inserted node data
//return:  null -> fail because it is full
void* ssz_list_emplace_front(SszList* container);
//insert node at head,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_list_push_front(SszList* container, const void* data);
//delete first node
void ssz_list_pop_front(SszList * container);

//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_list_emplace_back(SszList* container);
//insert node at tail,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_list_push_back(SszList* container, const void* data);
//delete tail node
void ssz_list_pop_back(SszList * container);

//insert the node at the position, return the insert data
//return:  null -> fail because it is full
void* ssz_list_emplace(SszList * container, int pos);
//insert the node at the position, set the node's data and return the insert pos
//return:  -1 -> fail because it is full
int ssz_list_insert(SszList * container, int pos, const void* data);

//delete the data at the pos, and return the next element pos
//if the pos is the last, it will return -1 
int ssz_list_erase(SszList * container, int pos);

//clear all 
void ssz_list_clear(SszList * container);

//Operations:
//remove the nodes which's value is equal with the data
void ssz_list_remove(SszList * container, const void* compare_data, SszListCompareFun compare_fun);

//delete the continue same nodes
void ssz_list_unique(SszList * container, SszListCompareFun compare_fun);

//data_tmp is used to save the tmp data
void ssz_list_swap(SszList * container, int pos1, int pos2);

//sort by the fun, data_tmp is used to save the tmp data, 
//it use ascending
void ssz_list_sort(SszList * container, SszListCompareFun compare_fun);

//sort by the fun, data_tmp is used to save the tmp data
void ssz_list_sort_by_descend(SszList * container, SszListCompareFun compare_fun);

//reverse the container
void ssz_list_reverse(SszList * container);


#ifdef __cplusplus
}
#endif