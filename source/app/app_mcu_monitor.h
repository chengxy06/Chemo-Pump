/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2018-04-25 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"

/************************************************
* Declaration
************************************************/
#define WATCHDOG_EXPIRED_TIME_MS 2000

void app_mcu_monitor_start();
void app_mcu_monitor_stop();
void app_mcu_monitor_stop_and_clear_reset_count();
void app_mcu_monitor_set_watchdog_expired_time(int expired_time);


