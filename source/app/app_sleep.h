/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-12-27 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "ssz_time.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

void app_sleep_enter(); 
//return: true->wake by time out, false->wake by other
bool app_sleep_enter_and_exit_at(SszDateTime * date_time);
//return: true->wake by time out, false->wake by other
bool app_sleep_enter_for_ms(int sleep_ms);
void app_sleep_exit();

#ifdef __cplusplus
}
#endif

