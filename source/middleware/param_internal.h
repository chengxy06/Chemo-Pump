/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-11-10 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "sys_power.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern BatteryLevel g_current_battery_level;
extern bool g_current_is_use_extern_power;
//extern bool g_current_battery_is_exist;
extern int g_encoder_count_of_one_ml;
extern bool g_is_pressure_adc_ready_when_installed;
extern int g_pressure_adc_when_installed;
extern bool g_is_pill_box_installed;




#ifdef __cplusplus
}
#endif

