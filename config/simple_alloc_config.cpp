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
#include "ssz_common.h"
#include "app_popup.h"
#include "app_alternative_popup.h"
#include "scr_tip_popup.h"

/************************************************
* Declaration
************************************************/
#define SALLOC_DEFINE_SIZE_AND_MEMORY(type, cache) cache,sizeof(cache[0]),sizeof(cache)/sizeof(cache[0]),type

typedef struct {
	const void* buff;
	int one_obj_memory_size;
	int obj_num;
	SAllocTypeID type;
}SAllocMemoryInfo;

/************************************************
* Variable 
************************************************/
static PopupObj g_alloc_popup_objs[2];
static AlternativePopupObj g_alloc_alternative_popup_objs[2];
static TipPopupObj g_alloc_tip_popup_objs[2];

const static SAllocMemoryInfo g_salloc_memory[] = {
	{ SALLOC_DEFINE_SIZE_AND_MEMORY(kSAllocPopupObj, g_alloc_popup_objs) },
	{ SALLOC_DEFINE_SIZE_AND_MEMORY(kSAllocAlternativePopupObj, g_alloc_alternative_popup_objs) },
	{ SALLOC_DEFINE_SIZE_AND_MEMORY(kSAllocTipPopupObj, g_alloc_tip_popup_objs) },
};
/************************************************
* Function 
************************************************/
void salloc_config() {
	int i;
	for (i = 0; i < ssz_array_size(g_salloc_memory); ++i) {
		for (int j = 0; j < g_salloc_memory[i].obj_num; ++j) {
			salloc_regist(g_salloc_memory[i].type, 
				(uint8_t*)(g_salloc_memory[i].buff) + j*g_salloc_memory[i].one_obj_memory_size,
				g_salloc_memory[i].one_obj_memory_size);
		}
	}
}