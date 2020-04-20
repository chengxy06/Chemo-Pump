/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-08 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "com_config.h"
#include "ssz_file.h"

/************************************************
* Declaration
************************************************/


#ifdef __cplusplus
extern "C" {
#endif


//usually called at interrupt handler
void drv_com_on_receive_one_byte(ComID com_id, unsigned char ch);

//return ssz file
SszFile* drv_com_file(ComID com_id);

void drv_com_init();



#ifdef __cplusplus
}
#endif



