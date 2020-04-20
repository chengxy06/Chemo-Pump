#include "ssz_vector.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/
#define M_get_node(container, index) ((uint8_t*)((container)->p_nodes + (index)*(container)->one_node_byte_size))
#define M_get_data_address(container,index) M_get_node(container, index)
#define M_get_data_size(container) ((container)->one_node_byte_size)
#define M_is_node_valid(node)
#define M_is_node_of_index_valid(container, index) (index>=0 && index<container->size)
#define M_is_index_valid(container, index) (index>=0 && index<container->max_size)
#define M_next_index(container, index) (index+1)
#define M_prev_index(container, index) (index-1)
#define M_tail_index(container) (container->size-1)
#define M_diff_index(container, index1,index2) (index1-index2)
/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/

//init by the node array and node array size
void ssz_vector_init(SszVector * container, void * p_nodes, int node_count, int one_node_byte_size) {
	ssz_assert(container && p_nodes);
	container->size = 0;
	container->max_size = (int16_t)node_count;
	container->one_node_byte_size = (int16_t)one_node_byte_size;
	container->p_nodes = (uint8_t*)p_nodes;
}

//Iterators:
//return the first node's position
//if return -1, it is indicate that it is empty
int ssz_vector_begin(SszVector * container) {
	ssz_assert(container);
	if (container->size > 0) {
		return 0;
	}else{
		return -1;
	}
}
//return the next node's pos
//if return -1, it is indicate that it arrive the end
int ssz_vector_next(SszVector * container, int pos) {
	int ret = -1;
	ssz_assert(container);
	if (container->size > 0 && M_is_index_valid(container, pos)){
		if (M_diff_index(container, pos, 0)+1 < container->size) {
			return M_next_index(container, pos);
		}
	}
	
	return ret;
}
//return the previous node's position
//if return -1, it is indicate that it arrive the begin
int ssz_vector_prev(SszVector * container, int pos){
	int ret = -1;

	ssz_assert(container);
	if (container->size > 0 && M_is_index_valid(container, pos)){
		if (M_diff_index(container, pos, 0) > 0) {
			return M_prev_index(container, pos);
		}
	}

	return ret;
}
//return the last node's position
//if return -1, it is indicate that it is empty
int ssz_vector_rbegin(SszVector * container){
	ssz_assert(container);
	if (container->size > 0) {
		return M_tail_index(container);
	}else{
		return -1;
	}
}


//Capacity:
//judge is empty
bool ssz_vector_is_empty(SszVector* container){
	ssz_assert(container);
	if (container->size <= 0) {
		return true;
	}else{
		return false;
	}
}
//judge is full
bool ssz_vector_is_full(SszVector* container){
	ssz_assert(container);
	if (container->size >= container->max_size) {
		return true;
	}else{
		return false;
	}	
}

int ssz_vector_size(SszVector* container){
	ssz_assert(container);
	return container->size;
}


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_vector_front(SszVector* container){
	ssz_assert(container);
	if (container->size>0) {
		return M_get_node(container, 0);
	}else{
		return NULL;
	}
}
//return last element's address
//return:  null->not exist
void* ssz_vector_back(SszVector* container){
	ssz_assert(container);
	if (container->size>0) {
		return M_get_node(container, M_tail_index(container));
	}else{
		return NULL;
	}	
}
//return the element's address at the pos
void* ssz_vector_at_pos(SszVector* container, int pos){
	ssz_assert(container);
	ssz_assert(M_is_index_valid(container, pos));
	ssz_assert(M_is_node_of_index_valid(container, pos));
	return M_get_node(container, pos);	
}

//return the element's address at the index, index range is 0~size-1
//return:  null->index is not right and it will assert
void* ssz_vector_at_index(SszVector* container, int index) {
	void* ret = NULL;
	ssz_assert(container);
	if (index>=0 && index<container->size)
	{
		ret = M_get_data_address(container, index);
	}
	else {		
		ret = NULL;
		ssz_assert_fail();
	}

	return ret;
}

int ssz_vector_pos_to_index(SszVector * container, int pos)
{
	return pos;
}

int ssz_vector_index_to_pos(SszVector * container, int index)
{
	if (index >= 0 && index < container->size)
	{
		return index;
	}
	return -1;
}


//Search:
//find the data's position
//return:  -1->not exist
int ssz_vector_find(SszVector * container, const void* data, SszVectorCompareFun compare_fun){
	ssz_assert(container);
	return ssz_vector_find_from(container, 0, data, compare_fun);
}
//find the data's position from the pos
//return:  -1->not exist
int ssz_vector_find_from(SszVector * container, int start_find_pos, const void* data, SszVectorCompareFun compare_fun){
	ssz_assert(container);
	int index = -1;

	//if it <0, find from begin
	if (start_find_pos <0) {
		start_find_pos = 0;
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

	        index = ssz_vector_next(container, index);
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
int ssz_vector_insert_internal(SszVector * container, int pos){
	ssz_assert(container);

	int new_index = -1;
	if (container->size < container->max_size) {
		if(container->size == 0){
			new_index = 0;
		}
		else if (pos >= container->size) {
			//insert at end
			if (container->size==0) {
				new_index = 0;
			}
			else {
				new_index = container->size;
			}
		}else if(container->size>0){
			if (pos<0) {
				pos = 0;
			}
			//insert at middle
			ssz_assert(M_is_node_of_index_valid(container, pos));
			//get the new end index
			int new_last_index = container->size;
			int pre_index;
			for (int i = new_last_index; i != pos;) {
				pre_index = M_prev_index(container, i);
				memcpy(M_get_data_address(container, i),
					M_get_data_address(container, pre_index), M_get_data_size(container));
				i--;
			}
			new_index = pos;
		}

		/* node size ++ */
		container->size++;
	}

	return new_index;	
}

//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_vector_emplace_back(SszVector* container){
	ssz_assert(container);
	return ssz_vector_emplace(container, container->max_size);
}
//insert node at tail,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_vector_push_back(SszVector* container, const void* data){
	ssz_assert(container);
	int ret;

	ret = ssz_vector_insert_internal(container, container->max_size);
	if (ret != -1) {
		memcpy(M_get_data_address(container,ret), data, M_get_data_size(container));
	}

	return ret;	
}
//delete tail node
void ssz_vector_pop_back(SszVector * container){
	ssz_assert(container);
	if (container->size>0) {
		ssz_vector_erase(container, M_tail_index(container));
	}
}

//insert the node at the position, return the insert data
//return:  null -> fail because it is full
void* ssz_vector_emplace(SszVector * container, int pos){
	ssz_assert(container);
	int ret;

	ret = ssz_vector_insert_internal(container, pos);
	if (ret != -1) {
		return M_get_data_address(container,ret);
	}

	return NULL;	
}
//insert the node at the position, set the node's data and return the insert pos
//return:  -1 -> fail because it is full
int ssz_vector_insert(SszVector * container, int pos, const void* data){
	ssz_assert(container);
	int ret;

	ret = ssz_vector_insert_internal(container, pos);
	if (ret != -1) {
		memcpy(M_get_data_address(container,ret), data, M_get_data_size(container));
	}

	return ret;		
}

//delete the data at the pos, and return the next element pos
//if the pos is the last, it will return -1 
int ssz_vector_erase(SszVector * container, int pos){
	ssz_assert(container);

	int ret = -1;
	if (M_is_index_valid(container, pos) && container->size>0) {
		ssz_assert(M_is_node_of_index_valid(container, pos));
		if (container->size == 1) {
			ret = -1;
		}
		else if (M_tail_index(container) == pos) {
			//do nothing
			ret = -1;
		}else {
			//get the new end index
			int tail_index = M_prev_index(container, M_tail_index(container));

			int next_index;
			for (int i = pos; i != tail_index;) {
				next_index = M_next_index(container, i);
				memcpy(M_get_data_address(container, i),
					M_get_data_address(container, next_index), M_get_data_size(container));
				i = M_next_index(container, i);
			}
			ret  = pos;
		}

		container->size--;
	}

	return ret;
}

//clear all 
void ssz_vector_clear(SszVector * container){
	ssz_assert(container);

	container->size = 0;
}

//Operations:
//remove the nodes which's value is equal with the data
void ssz_vector_remove(SszVector * container, const void* compare_data, SszVectorCompareFun compare_fun){
	ssz_assert(container);
	int pos;
	const void* user_data;
	for (pos=ssz_vector_begin(container); pos!=-1; ) {
		user_data = ssz_vector_at_pos(container, pos);
		if (compare_fun(user_data, compare_data, M_get_data_size(container))==0) {
			pos = ssz_vector_erase(container, pos);
		}else{
			pos = ssz_vector_next(container, pos);
		}
	}
}

//delete the continue same node
void ssz_vector_unique(SszVector * container, SszVectorCompareFun compare_fun){
	ssz_assert(container);
	int pos;
	const void* user_data;
	const void* compare_data = NULL;
	for (pos=ssz_vector_begin(container); pos!=-1; ) {
		user_data = ssz_vector_at_pos(container, pos);
		if(compare_data){
			if (compare_fun(user_data, compare_data, M_get_data_size(container))==0) {
				pos = ssz_vector_erase(container, pos);
				continue;
			}else{
				compare_data = user_data;
			}
		}
		else{
			//first pos, no need compare, just set it to compare data
			compare_data = user_data;
		}
		pos = ssz_vector_next(container, pos);
	}
}

//data_tmp is used to save the tmp data
void ssz_vector_swap(SszVector * container, int pos1, int pos2){
	ssz_assert(container&&M_is_node_of_index_valid(container, pos1)&& M_is_node_of_index_valid(container, pos2));
	void* data1 = ssz_vector_at_pos(container, pos1);
	void* data2 = ssz_vector_at_pos(container, pos2);
	void* data_tmp = ssz_tmp_buff_alloc(kTempAllocCommonBuff,container->one_node_byte_size);
	memcpy(data_tmp, data1, M_get_data_size(container));
	memcpy(data1, data2, M_get_data_size(container));
	memcpy(data2, data_tmp, M_get_data_size(container));
	ssz_tmp_buff_free(data_tmp);
}

//sort by the fun, data_tmp is used to save the tmp data, 
//it use ascending
void ssz_vector_sort_internal(SszVector * container, SszVectorCompareFun compare_fun, bool is_ascending){
	ssz_assert(container);
	int end_pos;
	end_pos = ssz_vector_rbegin(container);
	while (end_pos!= ssz_vector_begin(container))
	{
		int i = ssz_vector_begin(container);
		int next_pos = ssz_vector_next(container, i);
		int compare_ret;
		for (;i!=-1&&next_pos!=-1&&i!=end_pos; )
		{
			compare_ret = compare_fun(M_get_data_address(container, i), 
					M_get_data_address(container, next_pos), M_get_data_size(container));
			if ((is_ascending &&compare_ret>0) || (!is_ascending &&compare_ret<0) )
			{
				ssz_vector_swap(container, i, next_pos);
			}
			if (i == end_pos)
			{
				break;
			}
			i = ssz_vector_next(container, i);
			next_pos = ssz_vector_next(container, i);
		}
		end_pos = ssz_vector_prev(container, end_pos);
	}
}

//sort by the fun, data_tmp is used to save the tmp data, 
//it use ascending
void ssz_vector_sort(SszVector * container, SszVectorCompareFun compare_fun){
	ssz_vector_sort_internal(container, compare_fun, true);
}

//sort by the fun, data_tmp is used to save the tmp data, 
void ssz_vector_sort_by_descend(SszVector * container, SszVectorCompareFun compare_fun){
	ssz_vector_sort_internal(container, compare_fun, false);	
}
//reverse the container
void ssz_vector_reverse(SszVector * container){
	ssz_assert(container);
	int pos;
	int last_pos;

	if(container->size<=1){
		return;
	}
	for (pos=ssz_vector_begin(container),last_pos=ssz_vector_rbegin(container);pos!=-1;)
	{
		ssz_vector_swap(container, pos, last_pos);
		pos = ssz_vector_next(container, pos);
		if (pos == last_pos) {
			break;
		}
		last_pos = ssz_vector_prev(container, last_pos);
		if (pos == last_pos) {
			break;
		}
	}
}
