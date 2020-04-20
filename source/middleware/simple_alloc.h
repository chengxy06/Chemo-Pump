/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-16 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "simple_alloc_config.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

//regist the alloc memory
void salloc_regist(SAllocTypeID type, void* memory, int memory_size);
//alloc 
void* salloc_new(SAllocTypeID type, int *memory_size);
//free
void salloc_delete(void* memory);





#ifdef __cplusplus
}
#endif

