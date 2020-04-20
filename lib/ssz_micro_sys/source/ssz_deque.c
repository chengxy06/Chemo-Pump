#include "ssz_deque.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/
#define M_get_node(container, index) ((uint8_t*)((container)->p_nodes + (index)*(container)->one_node_byte_size))
#define M_get_data_address(container,index) M_get_node(container, index)
#define M_get_data_size(container) ((container)->one_node_byte_size)
#define M_is_node_valid(node)
#define M_is_node_of_index_valid(container, index) ssz_deque_is_node_of_index_valid(container, index)
#define M_is_index_valid(container, index) (index>=0 && index<container->max_size)
#define M_next_index(container, index) ssz_deque_next_index_simple(container, index)
#define M_prev_index(container, index) ssz_deque_prev_index_simple(container, index)
#define M_tail_index(container) ssz_deque_tail_index_simple(container)
#define M_diff_index(container, index1,index2) ssz_deque_diff_index_simple(container, index1, index2)
/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
static int16_t ssz_deque_next_index_simple(SszDeque * container, int16_t index)
{
	index++; 
	if (index >= container->max_size) {
		index -= container->max_size; 
	}

	return index;
}
static int16_t ssz_deque_prev_index_simple(SszDeque * container, int16_t index)
{
	index--; 
	if (index<0) {
		index += container->max_size; 
	}
	return index;
}

static int16_t ssz_deque_tail_index_simple(SszDeque * container)
{
	int16_t index = container->start_index + container->size-1;
	if (index >= container->max_size) {
		index -= container->max_size;
	}
	return index;
}
static int ssz_deque_diff_index_simple(SszDeque * container, int index1, int index2)
{
	int diff= index1 - index2;
	if (diff < 0) {
		diff += container->max_size;
	}
	return diff;
}
static bool ssz_deque_is_node_of_index_valid(SszDeque * container, int index)
{
	bool ret = false;
	if(index >= 0 && index<container->max_size && container->size>0)
	{
		int diff;
		diff = index - container->start_index;
		if (diff<0) {
			diff += container->max_size;
		}
		if (diff<container->size) {
			ret = true;
		}
	}

	return ret;
}

//init by the node array and node array size
void ssz_deque_init(SszDeque * container, void * p_nodes, int node_count, int one_node_byte_size) {
	ssz_assert(container && p_nodes);
	container->size = 0;
	container->max_size = (int16_t)node_count;
	container->start_index = 0;
	container->one_node_byte_size = (int16_t)one_node_byte_size;
	container->p_nodes = (uint8_t*)p_nodes;
}

//Iterators:
//return the first node's position
//if return -1, it is indicate that it is empty
int ssz_deque_begin(SszDeque * container) {
	ssz_assert(container);
	if (container->size > 0) {
		return container->start_index;
	}else{
		return -1;
	}
}
//return the next node's pos
//if return -1, it is indicate that it arrive the end
int ssz_deque_next(SszDeque * container, int pos) {
	int ret = -1;
	ssz_assert(container);
	if (container->size > 0 && M_is_index_valid(container, pos)){
		if (M_diff_index(container, pos, container->start_index)+1 < container->size) {
			return M_next_index(container, (int16_t)pos);
		}
	}
	
	return ret;
}
//return the previous node's position
//if return -1, it is indicate that it arrive the begin
int ssz_deque_prev(SszDeque * container, int pos){
	int ret = -1;

	ssz_assert(container);
	if (container->size > 0 && M_is_index_valid(container, pos)){
		if (M_diff_index(container, pos, container->start_index) > 0) {
			return M_prev_index(container, (int16_t)pos);
		}
	}

	return ret;
}
//return the last node's position
//if return -1, it is indicate that it is empty
int ssz_deque_rbegin(SszDeque * container){
	ssz_assert(container);
	if (container->size > 0) {
		return M_tail_index(container);
	}else{
		return -1;
	}
}


//Capacity:
//judge is empty
bool ssz_deque_is_empty(SszDeque* container){
	ssz_assert(container);
	if (container->size <= 0) {
		return true;
	}else{
		return false;
	}
}
//judge is full
bool ssz_deque_is_full(SszDeque* container){
	ssz_assert(container);
	if (container->size >= container->max_size) {
		return true;
	}else{
		return false;
	}	
}

int ssz_deque_size(SszDeque* container){
	ssz_assert(container);
	return container->size;
}


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_deque_front(SszDeque* container){
	ssz_assert(container);
	if (container->size>0) {
		return M_get_node(container, container->start_index);
	}else{
		return NULL;
	}
}
//return last element's address
//return:  null->not exist
void* ssz_deque_back(SszDeque* container){
	ssz_assert(container);
	if (container->size>0) {
		return M_get_node(container, M_tail_index(container));
	}else{
		return NULL;
	}	
}
//return the element's address at the pos
void* ssz_deque_at_pos(SszDeque* container, int pos){
	ssz_assert(container);
	ssz_assert(M_is_index_valid(container, pos));
	ssz_assert(M_is_node_of_index_valid(container, pos));
	return M_get_node(container, pos);	
}

//return the element's address at the index, index range is 0~size-1
//return:  null->index is not right and it will assert
void* ssz_deque_at_index(SszDeque* container, int index) {
	void* ret = NULL;
	ssz_assert(container);
	if (index>=0 && index<container->size)
	{
		int dest_index = index + container->start_index;
		if (dest_index >= container->max_size) {
			dest_index -= container->max_size;
		}
		ret = M_get_data_address(container, dest_index);
	}
	else {
		ret = NULL;
		ssz_assert_fail();
	}

	return ret;
}
int ssz_deque_pos_to_index(SszDeque * container, int pos)
{
	if (pos != -1) {
		int diff;
		diff = pos - container->start_index;
		if (diff < 0) {
			diff += container->max_size;
		}
		return diff;
	}
	return -1;
}

int ssz_deque_index_to_pos(SszDeque * container, int index)
{
	if (index >= 0 && index < container->size)
	{
		int pos = index + container->start_index;
		if (pos >= container->max_size) {
			pos -= container->max_size;
		}
		return pos;
	}
	return -1;
}


//Search:
//find the data's position
//return:  -1->not exist
int ssz_deque_find(SszDeque * container, const void* data, SszDequeCompareFun compare_fun){
	ssz_assert(container);
	return ssz_deque_find_from(container, container->start_index, data, compare_fun);
}
//find the data's position from the pos
//return:  -1->not exist
int ssz_deque_find_from(SszDeque * container, int start_find_pos, const void* data, SszDequeCompareFun compare_fun){
	ssz_assert(container);
	int index = -1;

	//if it <0, find from begin
	if (start_find_pos <0) {
		start_find_pos = container->start_index;
	}
	if (container->size>0 && M_is_index_valid(container, start_find_pos)){
		ssz_assert(	M_is_node_of_index_valid(container, start_find_pos));
	    index = start_find_pos;        

		int i;
	    for (i= 0;i<container->max_size&&index!=-1;i++){
	        if (compare_fun(M_get_node(container, index),
	        		 data, M_get_data_size(container))==0){
	            break;
	        }

	        index = ssz_deque_next(container, index);
	    }      

	    if (i>=container->max_size) {
	    	ssz_assert(index == -1);
	      	index = -1;
	    }  
	}
	return index;
}

//Modifiers
//insert the node at the position, return the insert pos
//return:  -1 -> fail because it is full
int16_t ssz_deque_insert_internal(SszDeque * container, int16_t pos){
	ssz_assert(container);

	int16_t new_index = -1;
	if (container->size < container->max_size) {
		if (pos >= container->max_size) {
			//insert at end
			if (container->size==0) {
				new_index = container->start_index;
			}
			else {
				new_index = container->start_index + container->size;
				if (new_index >= container->max_size) {
					new_index -= container->max_size;
				}
			}
		}else if (pos<0||pos ==container->start_index) {
			//insert before begin
			if (container->size == 0) {
				new_index = container->start_index;
			}
			else {
				new_index = container->start_index - 1;
				if (new_index < 0) {
					new_index += container->max_size;
				}
				container->start_index = new_index;
			}
		}else if(container->size>0){
			//insert at middle
			ssz_assert(M_is_node_of_index_valid(container, pos));
			//get the new end index
			int16_t new_last_index = container->start_index + container->size;
			if (new_last_index >= container->max_size) {
				new_last_index -= container->max_size;
			}
			int16_t pre_index;
			for (int16_t i = new_last_index; i != pos;) {
				pre_index = M_prev_index(container, i);
				memcpy(M_get_data_address(container, i),
					M_get_data_address(container, pre_index), M_get_data_size(container));
				i--;
				if (i<0) {
					i += container->max_size;
				}
			}
			new_index = pos;
		}

		/* node size ++ */
		container->size++;
	}

	return new_index;	
}

//insert node at head and return the inserted node data
//return:  null -> fail because it is full
void* ssz_deque_emplace_front(SszDeque* container){
	ssz_assert(container);
	return ssz_deque_emplace(container, container->start_index);
}
//insert node at head,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_deque_push_front(SszDeque* container, const void* data){
	ssz_assert(container);
	int ret;

	ret = ssz_deque_insert_internal(container, container->start_index);
	if (ret != -1) {
		memcpy(M_get_data_address(container,ret), data, M_get_data_size(container));
	}

	return ret;
}
//delete first node
void ssz_deque_pop_front(SszDeque * container){
	ssz_assert(container);
	if (container->size > 0) {
		ssz_deque_erase(container, container->start_index);
	}
}

//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_deque_emplace_back(SszDeque* container){
	ssz_assert(container);
	return ssz_deque_emplace(container, container->max_size);
}
//insert node at tail,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_deque_push_back(SszDeque* container, const void* data){
	ssz_assert(container);
	int ret;

	ret = ssz_deque_insert_internal(container, container->max_size);
	if (ret != -1) {
		memcpy(M_get_data_address(container,ret), data, M_get_data_size(container));
	}

	return ret;	
}
//delete tail node
void ssz_deque_pop_back(SszDeque * container){
	ssz_assert(container);
	if (container->size>0) {
		ssz_deque_erase(container, M_tail_index(container));
	}
}

//insert the node at the position, return the insert data
//return:  null -> fail because it is full
void* ssz_deque_emplace(SszDeque * container, int pos){
	ssz_assert(container);
	int16_t ret;

	ret = ssz_deque_insert_internal(container, (int16_t)pos);
	if (ret != -1) {
		return M_get_data_address(container,ret);
	}

	return NULL;	
}
//insert the node at the position, set the node's data and return the insert pos
//return:  -1 -> fail because it is full
int ssz_deque_insert(SszDeque * container, int pos, const void* data){
	ssz_assert(container);
	int16_t ret;

	ret = ssz_deque_insert_internal(container, (int16_t)pos);
	if (ret != -1) {
		memcpy(M_get_data_address(container,ret), data, M_get_data_size(container));
	}

	return ret;		
}

//delete the data at the pos, and return the next element pos
//if the pos is the last, it will return -1 
int ssz_deque_erase(SszDeque * container, int pos){
	ssz_assert(container);

	int16_t ret = -1;
	if (M_is_index_valid(container, pos) && container->size>0) {
		ssz_assert(M_is_node_of_index_valid(container, pos));
		if (container->size == 1) {
			container->start_index = 0;
			ret = -1;
		}
		else if (container->start_index == pos) {
			container->start_index = M_next_index(container, container->start_index);
			ret = container->start_index;
		}
		else if (M_tail_index(container) == pos) {
			//do nothing
			ret = -1;
		}else {
			//get the new end index
			int16_t tail_index = M_prev_index(container, M_tail_index(container));

			int16_t next_index;
			for (int16_t i = (int16_t)pos; i != tail_index;) {
				next_index = M_next_index(container, i);
				memcpy(M_get_data_address(container, i),
					M_get_data_address(container, next_index), M_get_data_size(container));
				i = M_next_index(container, i);
			}
			ret  = (int16_t)pos;
		}

		container->size--;
	}

	return ret;
}

//clear all 
void ssz_deque_clear(SszDeque * container){
	ssz_assert(container);

	container->size = 0;
	container->start_index = 0;
}

//Operations:
//remove the nodes which's value is equal with the data
void ssz_deque_remove(SszDeque * container, const void* compare_data, SszDequeCompareFun compare_fun){
	ssz_assert(container);
	int pos;
	const void* user_data;
	for (pos=ssz_deque_begin(container); pos!=-1; ) {
		user_data = ssz_deque_at_pos(container, pos);
		if (compare_fun(user_data, compare_data, M_get_data_size(container))==0) {
			pos = ssz_deque_erase(container, pos);
		}else{
			pos = ssz_deque_next(container, pos);
		}
	}
}

//delete the continue same node
void ssz_deque_unique(SszDeque * container, SszDequeCompareFun compare_fun){
	ssz_assert(container);
	int pos;
	const void* user_data;
	const void* compare_data = NULL;
	for (pos=ssz_deque_begin(container); pos!=-1; ) {
		user_data = ssz_deque_at_pos(container, pos);
		if(compare_data){
			if (compare_fun(user_data, compare_data, M_get_data_size(container))==0) {
				pos = ssz_deque_erase(container, pos);
				continue;
			}else{
				compare_data = user_data;
			}
		}
		else{
			//first pos, no need compare, just set it to compare data
			compare_data = user_data;
		}
		pos = ssz_deque_next(container, pos);
	}
}

//data_tmp is used to save the tmp data
void ssz_deque_swap(SszDeque * container, int pos1, int pos2){
	ssz_assert(container&&M_is_node_of_index_valid(container, pos1)&& M_is_node_of_index_valid(container, pos2));
	void* data1 = ssz_deque_at_pos(container, pos1);
	void* data2 = ssz_deque_at_pos(container, pos2);
	void* data_tmp = ssz_tmp_buff_alloc(kTempAllocCommonBuff, container->one_node_byte_size);
	memcpy(data_tmp, data1, M_get_data_size(container));
	memcpy(data1, data2, M_get_data_size(container));
	memcpy(data2, data_tmp, M_get_data_size(container));
	ssz_tmp_buff_free(data_tmp);
}

//sort by the fun, data_tmp is used to save the tmp data, 
//it use ascending
void ssz_deque_sort_internal(SszDeque * container, SszDequeCompareFun compare_fun, bool is_ascending){
	ssz_assert(container);
	int end_pos;
	end_pos = ssz_deque_rbegin(container);
	while (end_pos!= ssz_deque_begin(container))
	{
		int i = ssz_deque_begin(container);
		int next_pos = ssz_deque_next(container, i);
		int compare_ret;
		for (;i!=-1&&next_pos!=-1&&i!=end_pos; )
		{
			compare_ret = compare_fun(M_get_data_address(container, i), 
					M_get_data_address(container, next_pos), M_get_data_size(container));
			if ((is_ascending &&compare_ret>0) || (!is_ascending &&compare_ret<0) )
			{
				ssz_deque_swap(container, i, next_pos);
			}
			if (i == end_pos)
			{
				break;
			}
			i = ssz_deque_next(container, i);
			next_pos = ssz_deque_next(container, i);
		}
		end_pos = ssz_deque_prev(container, end_pos);
	}
}

//sort by the fun, data_tmp is used to save the tmp data, 
//it use ascending
void ssz_deque_sort(SszDeque * container, SszDequeCompareFun compare_fun){
	ssz_deque_sort_internal(container, compare_fun, true);
}

//sort by the fun, data_tmp is used to save the tmp data, 
void ssz_deque_sort_by_descend(SszDeque * container, SszDequeCompareFun compare_fun){
	ssz_deque_sort_internal(container, compare_fun, false);	
}
//reverse the container
void ssz_deque_reverse(SszDeque * container){
	ssz_assert(container);
	int pos;
	int last_pos;

	if(container->size<=1){
		return;
	}
	for (pos=ssz_deque_begin(container),last_pos=ssz_deque_rbegin(container);pos!=-1;)
	{
		ssz_deque_swap(container, pos, last_pos);
		pos = ssz_deque_next(container, pos);
		if (pos == last_pos) {
			break;
		}
		last_pos = ssz_deque_prev(container, last_pos);
		if (pos == last_pos) {
			break;
		}
	}
}
