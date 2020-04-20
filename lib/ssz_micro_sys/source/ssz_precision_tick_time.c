#include "ssz_precision_tick_time.h"
#include "ssz_tick_time.h"
#include "ssz_common.h"
#include "ssz_gpio.h"
/************************************************
* Declaration
************************************************/
#if SSZ_PRECISION_TICK_TIME_MODULE_ENABLE

/************************************************
* Variable 
************************************************/


/************************************************
* Function 
************************************************/

//return the now ms and the systick value since power on
//Note: it support max 24 days, after 24days, it will recount from 0
void ssz_precision_tick_time_now(SszPrecisionTickTime* now_time) {
#ifdef SSZ_TARGET_MACHINE
	now_time->time_of_ms = ssz_tick_time_now();
#ifdef SSZ_TARGET_SIMULATOR
	now_time->value_of_systick = 0;
#else
	now_time->value_of_systick = SysTick->VAL;
#endif
#else
	sim_precision_time_now(now_time);
#endif
}

//calc the elapsed time from old_time to now, unit: us
//the max time is about 35 minute
int32_t ssz_precision_tick_time_elapsed_us(SszPrecisionTickTime* old_time) {
	SszPrecisionTickTime now_time;
	ssz_precision_tick_time_now(&now_time);
	return ssz_precision_tick_time_elapsed_us_ex(old_time, &now_time);
}
//calc the elapsed time from old_time to new_time, unit: us
//the max time is about 35 minute
int32_t ssz_precision_tick_time_elapsed_us_ex(SszPrecisionTickTime* old_time, SszPrecisionTickTime* new_time) {
#ifdef SSZ_TARGET_MACHINE
	int32_t diff_ms;
	uint32_t diff_value;

	//get the escaped ms
	diff_ms = ssz_tick_time_elapsed_ex(old_time->time_of_ms, new_time->time_of_ms);

	//get the escaped value of systick
	if (new_time->value_of_systick <= old_time->value_of_systick)
	{
		diff_value = old_time->value_of_systick - new_time->value_of_systick;
	}
	else
	{
#ifdef SSZ_TARGET_SIMULATOR
		diff_value = 0;
#else
		diff_value = SysTick->LOAD - new_time->value_of_systick + old_time->value_of_systick;
		diff_ms -= 1;
#endif
	}

	return diff_ms * 1000 + diff_value / (ssz_system_clock() / 1000000);
#else
	return sim_precision_time_elapsed_us_ex(old_time, new_time);
#endif
}

//calc the elapsed time from old_time to new_time
void ssz_precision_tick_time_elapsed_ex(SszPrecisionTickTime* old_time, SszPrecisionTickTime* new_time, SszPrecisionTickTime* diff_time){
#ifdef SSZ_TARGET_MACHINE
	//uint32_t diff_value;

	//get the escaped ms
	diff_time->time_of_ms = ssz_tick_time_elapsed_ex(old_time->time_of_ms, new_time->time_of_ms);

	//get the escaped value of systick
	if (new_time->value_of_systick <= old_time->value_of_systick)
	{
		diff_time->value_of_systick = old_time->value_of_systick - new_time->value_of_systick;
	}
	else
	{
#ifdef SSZ_TARGET_SIMULATOR
		diff_time->value_of_systick = 0;
#else
		diff_time->value_of_systick = SysTick->LOAD - new_time->value_of_systick + old_time->value_of_systick;
		diff_time->time_of_ms -= 1;
#endif
	}
#else
	*diff_time = *new_time - *old_time;
#endif
}
#endif