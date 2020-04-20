/************************************************
* DESCRIPTION:
*   
************************************************/
#include "ssz_map.h"
#include "ssz_common.h"
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
void ssz_map_init(SszMap * container, SszMapNode * p_nodes, int node_count, bool is_use_index_as_key) {
	container->p_nodes = p_nodes;
	container->size = node_count;
	container->is_use_index_as_key = is_use_index_as_key;
	if (!is_use_index_as_key) {
		for (int i = 0; i < node_count; i++) {
			p_nodes[i].key = SSZ_MAP_INVALID_KEY;
		}
	}
}

//it will return -1 if not find the key
//if find the key, do not use first_invalid_node_index
//if not find any invalid node, the first_invalid_node_index will return -1
int ssz_map_find_by_key(const SszMap * container, int key, int16_t* first_invalid_node_index) {
	int16_t invalid_node_index = -1;
	for (int i = 0; i < container->size; i++) {
		if (container->p_nodes[i].key == key) {
			return i;
		}
		else if(invalid_node_index==-1 &&
			container->p_nodes[i].key == SSZ_MAP_INVALID_KEY){
			invalid_node_index = i;
		}
	}

	if (first_invalid_node_index) {
		*first_invalid_node_index = invalid_node_index;
	}
	return -1;
}

bool ssz_map_set_internal(SszMap * container, int key, void* value, int int_value, bool is_set_addr){

	if (container->is_use_index_as_key) {
		if (key<0 || key>=container->size) {
			return false;
		}
		else {
			if (is_set_addr) {
				container->p_nodes[key].user_data.value_addr = value;
			}
			else {
				container->p_nodes[key].user_data.int_value = int_value;
			}
			return true;
		}
	}
	else {

		int16_t invalid_node_index = -1;
		int index = ssz_map_find_by_key(container, key, &invalid_node_index);
		if (index != -1) {
			//find the key
			if (is_set_addr) {
				container->p_nodes[index].user_data.value_addr = value;
			}
			else {
				container->p_nodes[index].user_data.int_value = int_value;
			}
		}
		else if (invalid_node_index != -1) {
			//not find the key, add it
			container->p_nodes[invalid_node_index].key = key;
			if (is_set_addr) {
				container->p_nodes[invalid_node_index].user_data.value_addr = value;
			}
			else {
				container->p_nodes[invalid_node_index].user_data.int_value = int_value;
			}
		}

		if (index == -1 && invalid_node_index == -1) {
			return false;
		}
		else {
			return true;
		}
	}

}

bool ssz_map_set(SszMap * container, int key, void* value) {
	return ssz_map_set_internal(container, key, value, 0, true);
}
bool ssz_map_set_int(SszMap * container, int key, int value) {
	return ssz_map_set_internal(container, key, NULL, value, false);

}
void ssz_map_clear(SszMap * container, int key) {
	if (container->is_use_index_as_key) {
		if (key < 0 || key >= container->size) {
		}
		else {
			container->p_nodes[key].user_data.value_addr = NULL;
		}
	}
	else {

		//int16_t invalid_node_index = -1;
		int index = ssz_map_find_by_key(container, key, NULL);
		if (index != -1) {
			//find the key
			container->p_nodes[index].user_data.value_addr = NULL;
			container->p_nodes[index].key = SSZ_MAP_INVALID_KEY;
		}
	}
}
//it will return NULL if not find the key
void* ssz_map_get(const SszMap * container, int key) {
	int index = ssz_map_find_by_key(container, key, NULL);
	if (index != -1) {
		//find the key
		return container->p_nodes[index].user_data.value_addr;
	}
	else {
		return NULL;
	}
}
//it will return 0 if not find the key
int ssz_map_get_int(const SszMap * container, int key) {
	int index = ssz_map_find_by_key(container, key, NULL);
	if (index != -1) {
		//find the key
		return container->p_nodes[index].user_data.int_value;
	}
	else {
		return 0;
	}
}

SszMapNode* ssz_map_at(SszMap * container, int index) {
	ssz_assert(index >= 0 && index < container->size);

	return &container->p_nodes[index];
}
int ssz_map_size(const SszMap * container) {
	return container->size;
}
//return the size which have valid key 
int ssz_map_valid_size(const SszMap * container) {
	int ret = 0;
	if (container->is_use_index_as_key) {
		ret = container->size;
	}
	else {
		for (int i = 0; i < container->size; i++) {
			if (container->p_nodes[i].key != SSZ_MAP_INVALID_KEY) {
				ret++;
			}
		}
	}

	return ret;
}