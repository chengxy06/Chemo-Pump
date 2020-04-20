#include "ssz_list.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/
#define INVALID_LIST_INDEX -13108
#define M_get_node(container, index) ((SszListNode*)((container)->p_nodes + (index)*(container)->one_node_byte_size))
#define M_get_data_address(container,index) &(M_get_node(container, index)->user_data)
#define M_get_data_size(container) ((container)->one_node_byte_size-4)
#define M_is_node_valid(node) (((SszListNode*)(node))->prev_index == INVALID_LIST_INDEX &&((SszListNode*)(node))->next_index == INVALID_LIST_INDEX?0:1)
#define M_is_node_of_index_valid(container, index) ((index==container->head_index) || M_is_node_valid( (M_get_node(container, index)) ))
#define M_is_index_valid(container, index) (index>=0 && index<container->max_size)
/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/


//init by the node array and node array size
void ssz_list_init(SszList * container, SszListNode * p_nodes, int node_count, int one_node_byte_size) {
	ssz_assert(container && p_nodes);
	container->size = 0;
	container->max_size = (int16_t)node_count;
	container->head_index = -1;
	container->tail_index = -1;
	container->one_node_byte_size = (int16_t)one_node_byte_size;
	container->p_nodes = (uint8_t*)p_nodes;

	SszListNode* node;
	//set all node as invalid
	for (int i=0; i<node_count; i++) {
		node = M_get_node(container, i);
		node->prev_index = INVALID_LIST_INDEX;
		node->next_index = INVALID_LIST_INDEX;
	}
}

//Iterators:
//return the first node's position
//if return -1, it is indicate that it is empty
int ssz_list_begin(SszList * container) {
	ssz_assert(container);
	if (container->size > 0) {
		return container->head_index;
	}else{
		return -1;
	}
}
//return the next node's pos
//if return -1, it is indicate that it arrive the end
int ssz_list_next(SszList * container, int pos) {

	ssz_assert(container);
	if (M_is_index_valid(container, pos)){
		return M_get_node(container, pos)->next_index;		
	}
	else{
		return -1;
	}
}
//return the previous node's position
//if return -1, it is indicate that it arrive the begin
int ssz_list_prev(SszList * container, int pos){
	ssz_assert(container);
	if (M_is_index_valid(container, pos)){
		return M_get_node(container, pos)->prev_index;		
	}
	else{
		return -1;
	}
}
//return the last node's position
//if return -1, it is indicate that it is empty
int ssz_list_rbegin(SszList * container){
	ssz_assert(container);
	if (container->size > 0) {
		return container->tail_index;
	}else{
		return -1;
	}
}


//Capacity:
//judge is empty
bool ssz_list_is_empty(SszList* container){
	ssz_assert(container);
	if (container->size <= 0) {
		return true;
	}else{
		return false;
	}
}
//judge is full
bool ssz_list_is_full(SszList* container){
	ssz_assert(container);
	if (container->size >= container->max_size) {
		return true;
	}else{
		return false;
	}	
}

int ssz_list_size(SszList* container){
	ssz_assert(container);
	return container->size;
}


//Element access:
//return first element's address, 
//return:  null->not exist 
void* ssz_list_front(SszList* container){
	ssz_assert(container);
	if (container->size>0) {
		return &(M_get_node(container, container->head_index)->user_data);
	}else{
		return NULL;
	}
}
//return last element's address
//return:  null->not exist
void* ssz_list_back(SszList* container){
	ssz_assert(container);
	if (container->size>0) {
		return &(M_get_node(container, container->tail_index)->user_data);
	}else{
		return NULL;
	}	
}
//return the element's address at the pos
void* ssz_list_at_pos(SszList* container, int pos){
	ssz_assert(container);
	ssz_assert(M_is_index_valid(container, pos));
	ssz_assert(M_is_node_of_index_valid(container, pos));
	return &(M_get_node(container, pos)->user_data);	
}

//Search:
//find the data's position
//return:  -1->not exist
int ssz_list_find(SszList * container, const void* data, SszListCompareFun compare_fun){
	ssz_assert(container);
	return ssz_list_find_from(container, container->head_index, data, compare_fun);
}
//find the data's position from the pos
//return:  -1->not exist
int ssz_list_find_from(SszList * container, int start_find_pos, const void* data, SszListCompareFun compare_fun){
	ssz_assert(container);
	int index = -1;

	//if it <0, find from begin
	if (start_find_pos <0) {
		start_find_pos = container->head_index;
	}
	if (container->size>0 && M_is_index_valid(container, start_find_pos)){
		ssz_assert(	M_is_node_of_index_valid(container, start_find_pos));
	    index = start_find_pos;        

		int i;
	    for (i= 0;i<container->max_size&&index!=-1;i++){
	        if (compare_fun(&(M_get_node(container, index)->user_data),
	        		 data, M_get_data_size(container))==0){
	            break;
	        }

	        index = M_get_node(container, index)->next_index;
	    }      

	    if (i>=container->max_size) {
	    	ssz_assert(index == -1);
	      	index = -1;
	    }  
	}
	return index;
}

static int16_t ssz_list_find_unuse_node(SszList* container){
	int16_t ret = -1;

	ssz_assert(container);

	if (container->size<container->max_size) {
		for (int16_t i = 0; i < container->max_size; ++i) {
			if ( !M_is_node_valid(M_get_node(container, i)) &&i!=container->head_index ) {
				ret = i;
				break;
			}
		}
	}

	return ret;
}

//Modifiers
//insert the node at the position, return the insert pos
//return:  -1 -> fail because it is full
int16_t ssz_list_insert_internal(SszList * container, int pos){
	ssz_assert(container);

	//find unused node
	int16_t new_index = ssz_list_find_unuse_node(container);
	if (new_index != -1) {

		int16_t prev_index = -1;
		int16_t next_index = -1;

		if (pos >= container->max_size) {
			//insert after end
			if(container->size>0){
				prev_index = container->tail_index;
			}
		}else if (pos<0) {
			//insert before begin
			if(container->size>0){
				next_index = container->head_index;
			}			
		}else{
			ssz_assert(M_is_node_of_index_valid(container, pos));
			next_index = (int16_t)pos;
			prev_index = M_get_node(container, pos)->prev_index;
		}

		SszListNode* new_node = M_get_node(container, new_index);
		//set the new node index
		new_node->prev_index = prev_index;
		new_node->next_index = next_index;

		//update prev and next node
		if (prev_index!=-1 && next_index!=-1) {
			//prev and next is valid
			M_get_node(container,prev_index)->next_index = new_index;
			M_get_node(container,next_index)->prev_index = new_index;
		}else if(prev_index == -1 && next_index == -1){
			//it is empty, update head and tail
			container->head_index = new_index;
			container->tail_index = new_index;
		}else if(prev_index == -1 && next_index != -1){
			//insert at begin, update head 
			M_get_node(container,next_index)->prev_index = new_index;
			container->head_index = new_index;
		}else if(prev_index != -1 && next_index == -1){
			//insert at end, update tail 
			M_get_node(container,prev_index)->next_index = new_index;
			container->tail_index = new_index;
		}

		/* node size ++ */
		container->size++;
	}

	return new_index;	
}

//insert node at head and return the inserted node data
//return:  null -> fail because it is full
void* ssz_list_emplace_front(SszList* container){
	ssz_assert(container);
	return ssz_list_emplace(container, container->head_index);
}
//insert node at head,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_list_push_front(SszList* container, const void* data){
	ssz_assert(container);
	int ret;

	ret = ssz_list_insert_internal(container, container->head_index);
	if (ret != -1) {
		memcpy(M_get_data_address(container,ret), data, M_get_data_size(container));
	}

	return ret;
}
//delete first node
void ssz_list_pop_front(SszList * container){
	ssz_assert(container);
	ssz_list_erase(container, container->head_index);
}

//insert node at tail and return the inserted node data
//return:  null -> fail because it is full
void* ssz_list_emplace_back(SszList* container){
	ssz_assert(container);
	return ssz_list_emplace(container, container->max_size);
}
//insert node at tail,set the node's data  and return the inserted node pos
//return:  -1 -> fail because it is full
int ssz_list_push_back(SszList* container, const void* data){
	ssz_assert(container);
	int ret;

	ret = ssz_list_insert_internal(container, container->max_size);
	if (ret != -1) {
		memcpy(M_get_data_address(container,ret), data, M_get_data_size(container));
	}

	return ret;	
}
//delete tail node
void ssz_list_pop_back(SszList * container){
	ssz_assert(container);
	ssz_list_erase(container, container->tail_index);
}

//insert the node at the position, return the insert data
//return:  null -> fail because it is full
void* ssz_list_emplace(SszList * container, int pos){
	ssz_assert(container);
	int ret;

	ret = ssz_list_insert_internal(container, pos);
	if (ret != -1) {
		return M_get_data_address(container,ret);
	}

	return NULL;	
}
//insert the node at the position, set the node's data and return the insert pos
//return:  -1 -> fail because it is full
int ssz_list_insert(SszList * container, int pos, const void* data){
	ssz_assert(container);
	int ret;

	ret = ssz_list_insert_internal(container, pos);
	if (ret != -1) {
		memcpy(M_get_data_address(container,ret), data, M_get_data_size(container));
	}

	return ret;		
}

//delete the data at the pos, and return the next element pos
//if the pos is the last, it will return -1 
int ssz_list_erase(SszList * container, int pos){
	ssz_assert(container);

	int16_t next_index = -1;
	int16_t prev_index;
	if (M_is_index_valid(container, pos) && container->size>0) {
		ssz_assert(M_is_node_of_index_valid(container, pos));
		prev_index = M_get_node(container,pos)->prev_index;
		next_index = M_get_node(container,pos)->next_index;

		M_get_node(container,pos)->prev_index = INVALID_LIST_INDEX;
		M_get_node(container,pos)->next_index = INVALID_LIST_INDEX;
		if (prev_index!=-1) {
			M_get_node(container, prev_index)->next_index = next_index; 
		}
		if (next_index!=-1) {
			M_get_node(container, next_index)->prev_index = prev_index; 
		}
		if (prev_index==-1) {
			//update head
			container->head_index = next_index;
		}
		if (next_index==-1) {
			//update tail
			container->tail_index = prev_index;
		}

		container->size--;
	}

	return next_index;
}

//clear all 
void ssz_list_clear(SszList * container){
	ssz_assert(container);

	container->size = 0;
	container->head_index = -1;
	container->tail_index = -1;

	SszListNode* node;
	//set all node as invalid
	for (int i=0; i<container->max_size; i++) {
		node = M_get_node(container, i);
		node->prev_index = INVALID_LIST_INDEX;
		node->next_index = INVALID_LIST_INDEX;
	}
}

//Operations:
//remove the nodes which's value is equal with the data
void ssz_list_remove(SszList * container, const void* compare_data, SszListCompareFun compare_fun){
	ssz_assert(container);
	int pos;
	const void* user_data;
	for (pos=ssz_list_begin(container); pos!=-1; ) {
		user_data = ssz_list_at_pos(container, pos);
		if (compare_fun(user_data, compare_data, M_get_data_size(container))==0) {
			pos = ssz_list_erase(container, pos);
		}else{
			pos = ssz_list_next(container, pos);
		}
	}
}

//delete the continue same node
void ssz_list_unique(SszList * container, SszListCompareFun compare_fun){
	ssz_assert(container);
	int pos;
	const void* user_data;
	const void* compare_data = NULL;
	for (pos=ssz_list_begin(container); pos!=-1; ) {
		user_data = ssz_list_at_pos(container, pos);
		if(compare_data){
			if (compare_fun(user_data, compare_data, M_get_data_size(container))==0) {
				pos = ssz_list_erase(container, pos);
				continue;
			}else{
				compare_data = user_data;
			}
		}
		else{
			//first pos, no need compare, just set it to compare data
			compare_data = user_data;
		}
		pos = ssz_list_next(container, pos);
	}
}

//data_tmp is used to save the tmp data
void ssz_list_swap(SszList * container, int pos1, int pos2){
	ssz_assert(container&&M_is_node_of_index_valid(container, pos1)&& M_is_node_of_index_valid(container, pos2));
	void* data1 = ssz_list_at_pos(container, pos1);
	void* data2 = ssz_list_at_pos(container, pos2);
	void* data_tmp = ssz_tmp_buff_alloc(kTempAllocCommonBuff, container->one_node_byte_size);
	memcpy(data_tmp, data1, M_get_data_size(container));
	memcpy(data1, data2, M_get_data_size(container));
	memcpy(data2, data_tmp, M_get_data_size(container));
	ssz_tmp_buff_free(data_tmp);
}

//sort by the fun, data_tmp is used to save the tmp data, 
//it use ascending
void ssz_list_sort_internal(SszList * container, SszListCompareFun compare_fun, bool is_ascending){
	ssz_assert(container);
	int end_pos;
	end_pos = ssz_list_rbegin(container);
	while (end_pos!= ssz_list_begin(container))
	{
		int i = ssz_list_begin(container);
		int next_pos = ssz_list_next(container, i);
		int compare_ret;
		for (;i!=-1&&next_pos!=-1&&i!=end_pos; )
		{
			compare_ret = compare_fun(M_get_data_address(container, i), 
					M_get_data_address(container, next_pos), M_get_data_size(container));
			if ((is_ascending &&compare_ret>0) || (!is_ascending &&compare_ret<0) )
			{
				ssz_list_swap(container, i, next_pos);
			}
			if (i == end_pos)
			{
				break;
			}
			i = ssz_list_next(container, i);
			next_pos = ssz_list_next(container, i);
		}
		end_pos = ssz_list_prev(container, end_pos);
	}
}

//sort by the fun, data_tmp is used to save the tmp data, 
//it use ascending
void ssz_list_sort(SszList * container, SszListCompareFun compare_fun){
	ssz_list_sort_internal(container, compare_fun, true);
}

//sort by the fun, data_tmp is used to save the tmp data, 
void ssz_list_sort_by_descend(SszList * container, SszListCompareFun compare_fun){
	ssz_list_sort_internal(container, compare_fun, false);	
}
//reverse the container
void ssz_list_reverse(SszList * container){
	ssz_assert(container);
	int16_t pos;
	int16_t next_pos;
	int16_t tmp_pos;
	SszListNode* p;
	if(container->size<=1){
		return;
	}
	for (pos=(int16_t)ssz_list_begin(container);pos!=-1;)
	{
		next_pos = (int16_t)ssz_list_next(container, pos);
		p = M_get_node(container, pos);
		tmp_pos = p->prev_index;
		p->prev_index = p->next_index;
		p->next_index = tmp_pos;
        pos = next_pos;
	}
	tmp_pos = container->head_index;
	container->head_index = container->tail_index;
	container->tail_index = tmp_pos;
}
