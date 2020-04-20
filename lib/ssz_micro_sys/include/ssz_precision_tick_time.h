#pragma once
#include "ssz_def.h"
#include "ssz_config.h"

/************************************************
* Declaration
************************************************/

#if SSZ_PRECISION_TICK_TIME_MODULE_ENABLE

#ifdef __cplusplus
extern "C" {
#endif

//the precision time to indicate the time with the ms and the value of systick
#ifdef SSZ_TARGET_MACHINE
typedef struct
{
    int32_t time_of_ms; //precision is MSECS_PER_TICK
    uint32_t value_of_systick; //SysTick Current Value 
}SszPrecisionTickTime;
#else
	typedef long long SszPrecisionTickTime;
#endif

//return the now ms and the systick value since power on
//Note: it support max 24 days, after 24days, it will recount from 0
void ssz_precision_tick_time_now(SszPrecisionTickTime* now_time);

//calc the elapsed time from old_time to now, unit: us
//the max time is about 35 minute
int32_t ssz_precision_tick_time_elapsed_us(SszPrecisionTickTime* old_time);

//calc the elapsed time from old_time to new_time, unit: us
//the max time is about 35 minute
int32_t ssz_precision_tick_time_elapsed_us_ex(SszPrecisionTickTime* old_time, SszPrecisionTickTime* new_time);

//calc the elapsed time from old_time to new_time
void ssz_precision_tick_time_elapsed_ex(SszPrecisionTickTime* old_time, SszPrecisionTickTime* new_time, SszPrecisionTickTime* diff_time);

#ifdef __cplusplus
}
#endif

#endif
