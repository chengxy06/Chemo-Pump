/************************************************
* DESCRIPTION:
*   
************************************************/
#pragma once
#include "ssz_def.h"
#include <limits.h>

/************************************************
* Declaration
************************************************/
#define SSZ_MAP_INVALID_KEY INT_MAX
typedef struct {
	int	key;
	union{
		int int_value;
		void* value_addr;
	}user_data;
}SszMapNode;

typedef struct {
	int16_t size;   		/* size */
	SszMapNode* p_nodes; 		/* node array */
	bool is_use_index_as_key;
}SszMap;

#ifdef __cplusplus
extern "C" {
#endif

//init by the node array and node array size
void ssz_map_init(SszMap * container, SszMapNode * p_nodes, int node_count, bool is_use_index_as_key);

bool ssz_map_set(SszMap * container, int key, void* value);
bool ssz_map_set_int(SszMap * container, int key, int value);
void ssz_map_clear(SszMap * container, int key);

//it will return NULL if not find the key
void* ssz_map_get(const SszMap * container, int key);
//it will return 0 if not find the key
int ssz_map_get_int(const SszMap * container, int key);

SszMapNode* ssz_map_at(SszMap * container, int index);
int ssz_map_size(const SszMap * container);
//return the size which have valid key 
int ssz_map_valid_size(const SszMap * container);

#ifdef __cplusplus
}
#endif