/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-19 dczhang
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"

/************************************************
* Declaration
************************************************/
 extern bool g_is_sleep_allowed;

void app_system_monitor_start();
void app_system_monitor_stop();

//no_operate_time_ms: when <=0, it will no sleep
void app_system_monitor_set_no_operate_time(int no_operate_time_ms);
void app_system_monitor_set_to_default_no_operate_time();


