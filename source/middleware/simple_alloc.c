/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-16 xqzhao
* Initial revision.
*
************************************************/
#include "simple_alloc.h"
#include "ssz_list.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/
typedef struct {
	void* memory;
	int16_t memory_size;
	SAllocTypeID type;
	bool is_used;
}SAllocInfo;

/************************************************
* Variable 
************************************************/
static SAllocInfo g_salloc_info_arr[SIMPLE_ALLOC_MAX_COUNT];

/************************************************
* Function 
************************************************/
void salloc_regist(SAllocTypeID type, void * memory, int memory_size)
{
	int i;
	for (i = 0; i < ssz_array_size(g_salloc_info_arr); i++) {
		if (g_salloc_info_arr[i].memory == NULL) {
			g_salloc_info_arr[i].is_used = false;
			g_salloc_info_arr[i].type = type;
			g_salloc_info_arr[i].memory = memory;
			g_salloc_info_arr[i].memory_size = memory_size;
			break;
		}
	}
	ssz_assert(i < ssz_array_size(g_salloc_info_arr));
}

void * salloc_new(SAllocTypeID type, int * memory_size)
{
	int i;
	for (i = 0; i < ssz_array_size(g_salloc_info_arr) && g_salloc_info_arr[i].memory; i++) {
		if (g_salloc_info_arr[i].type == type &&
			g_salloc_info_arr[i].is_used == false) {
			g_salloc_info_arr[i].is_used = true;
			if(memory_size){
				*memory_size = g_salloc_info_arr[i].memory_size;
			}
			return g_salloc_info_arr[i].memory;
		}
	}

	return NULL;
}

void salloc_delete(void * memory)
{
	int i;
	for (i = 0; i < ssz_array_size(g_salloc_info_arr) && g_salloc_info_arr[i].memory; i++) {
		if (g_salloc_info_arr[i].memory == memory) {
			g_salloc_info_arr[i].is_used = false;
			break;
		}
	}
}
