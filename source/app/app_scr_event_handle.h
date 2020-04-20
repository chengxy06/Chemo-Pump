/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-09 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "key.h"

/************************************************
* Declaration
************************************************/

//register screen monitor event
void app_scr_event_handle_init();

void app_scr_on_release_power_key(KeyID key, int repeat_count);
