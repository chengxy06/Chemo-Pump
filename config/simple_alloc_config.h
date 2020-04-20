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

/************************************************
* Declaration
************************************************/
#define SIMPLE_ALLOC_MAX_COUNT 10

typedef enum {
	kSAllocPopupObj,
	kSAllocAlternativePopupObj,
	kSAllocTipPopupObj,
	kSAllocTypeIDMax
}SAllocTypeID;

#ifdef __cplusplus
extern "C" {
#endif
void salloc_config();
#ifdef __cplusplus
}
#endif