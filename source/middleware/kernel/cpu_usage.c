/************************************************
* DESCRIPTION:
*
************************************************/
#include "cpu_usage.h"
#include "ssz_gpio.h"
#include "ssz_precision_tick_time.h"
#include "config.h"
#include "ssz_common.h"
/************************************************
* Declaration
************************************************/
#if CPU_USAGE_MODULE_ENABLE
/************************************************
* Variable 
************************************************/
SszPrecisionTickTime g_cpu_usage_start_time;
SszPrecisionTickTime g_cpu_usage_idle_start_time;
#ifdef SSZ_TARGET_MACHINE
int g_cpu_usage_idle_time;
int g_cpu_usage_idle_value_of_systick;
#else
SszPrecisionTickTime g_cpu_usage_idle_time;
#endif
/************************************************
* Function 
************************************************/
//when call it, it will reset usage
void cpu_usage_print_if_large_than(int compare_usage) {
#ifdef SSZ_TARGET_MACHINE
	uint32_t tick_load = SysTick->LOAD;
#else
	uint32_t tick_load = 1;
#endif
	SszPrecisionTickTime all_time;
	SszPrecisionTickTime curr_time;
	ssz_precision_tick_time_now(&curr_time);
	ssz_precision_tick_time_elapsed_ex(&g_cpu_usage_start_time,
		&curr_time,&all_time);
	SszPrecisionTickTime used_time;
#ifdef SSZ_TARGET_MACHINE
	used_time.time_of_ms = all_time.time_of_ms - g_cpu_usage_idle_time;
	if (all_time.value_of_systick>=g_cpu_usage_idle_value_of_systick) {
		used_time.value_of_systick = all_time.value_of_systick - g_cpu_usage_idle_value_of_systick;
	}
	else {
		used_time.time_of_ms--;
		used_time.value_of_systick = tick_load - g_cpu_usage_idle_value_of_systick+ all_time.value_of_systick;
	}
#else
	used_time = all_time - g_cpu_usage_idle_time;
#endif

	int usage = 0;
#ifdef SSZ_TARGET_MACHINE
	if (all_time.time_of_ms>60000) {
		usage = used_time.time_of_ms * 100 / all_time.time_of_ms;
	}
	else {
		if (all_time.time_of_ms==0 && all_time.value_of_systick==0) {
			usage = 0;
		}
		else {
			usage = (used_time.time_of_ms*tick_load + used_time.value_of_systick) *
				100 / (all_time.time_of_ms*tick_load + all_time.value_of_systick);
		}
	}
#else
	usage = used_time*100.0 / all_time;
#endif
	if (usage >= compare_usage) {
		printf("CPU usage:%d%%\n", usage);
#ifdef SSZ_TARGET_MACHINE
		printf("Total:%0.3fms, used:%0.3fms\n", all_time.time_of_ms + all_time.value_of_systick*1.0 / tick_load*MSECS_PER_TICK,
			used_time.time_of_ms + used_time.value_of_systick*1.0 / tick_load*MSECS_PER_TICK);
#else
		printf("Total:%0.3fms, used:%0.3fms\n", sim_precison_time_to_us(&all_time)/1000.0,
			sim_precison_time_to_us(&used_time)/1000.0);
#endif
	}
	cpu_usage_reset_usage();
}
void cpu_usage_reset_usage() {
	ssz_precision_tick_time_now(&g_cpu_usage_start_time);
	g_cpu_usage_idle_time = 0;
#ifdef SSZ_TARGET_MACHINE
	g_cpu_usage_idle_value_of_systick = 0;
#endif
}
void cpu_usage_enter_idle() {
	ssz_precision_tick_time_now(&g_cpu_usage_idle_start_time);
}
void cpu_usage_exit_idle() {
	SszPrecisionTickTime idle_end_time;
	ssz_precision_tick_time_now(&idle_end_time);
	SszPrecisionTickTime ret;

	ssz_precision_tick_time_elapsed_ex(&g_cpu_usage_idle_start_time,
		&idle_end_time, &ret);
#ifdef SSZ_TARGET_MACHINE
	g_cpu_usage_idle_time += ret.time_of_ms;
	g_cpu_usage_idle_value_of_systick += ret.value_of_systick;
	while(g_cpu_usage_idle_value_of_systick>SysTick->LOAD) {
		g_cpu_usage_idle_value_of_systick -= SysTick->LOAD;
		g_cpu_usage_idle_time++;
	}
#else
	g_cpu_usage_idle_time += ret;
#endif
}
#endif