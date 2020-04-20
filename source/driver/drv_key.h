/************************************************
* DESCRIPTION:
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "key_config.h"

/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif


//it will scan all key state and put at the param
void drv_key_scan(int8_t key_state[kKeyIDMax]);
bool drv_key_is_pressed(KeyID key);
bool drv_key_is_high(KeyID key);

#ifdef __cplusplus
}
#endif


