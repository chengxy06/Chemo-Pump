/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-12-27 xqzhao
* Initial revision.
*
************************************************/
#include "app_sleep.h"
#include "drv_lowpower.h"
#include "ssz_common.h"
#include "ssz_time_utility.h"
#include "limits.h"
#include "timer.h"
#include "drv_rtc.h"
#include "drv_key.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "sim_interface.h"
#endif
#include "drv_interrupt.h"
#include "alarm_play.h"
#include "ssz_gpio.h"
#include "mid_common.h"
/************************************************
* Declaration
************************************************/
#define WAKE_UP_INTERVAL_TIME_MS 125

#define ENABLE_CHECK_RTC_TO_CORRECT_REST_SLEEP_TIME 1
#define TIMER_DEVIATION_SECS_EACH_MINUTE 1

#define ENABLE_CHECK_RTC_TO_WAKE_UP 0


#define LOW_POWER_UART_ENABLE 0


/************************************************
* Variable 
************************************************/
static bool g_sleep_is_need_exit;
#if LOW_POWER_UART_ENABLE
static int32_t g_sleep_last_check_rtc_time;
#endif
/************************************************
* Function 
************************************************/
#ifdef SSZ_TARGET_SIMULATOR
bool app_sleep_for_ms(int sleep_ms) {
	sim_timer_clear("systick");
	while (sleep_ms > 0) {
		sim_delay_1ms();
		sleep_ms--;
		if (drv_key_is_pressed(kKeyOK)) {
			g_interrupt_is_confirm_key_pressed = true;
			return false;
		}
	}

	return true;
}
#endif

int app_sleep_diff_secs_from_now_to(int32_t wake_up_time)
{
	int32_t curr_time_secs;
#ifdef SSZ_TARGET_MACHINE
	ssz_gpio_enable_clock(RTC_I2C_SCK_GPIO_Port);
	ssz_gpio_set_mode(RTC_I2C_SCK_GPIO_Port, RTC_I2C_SCK_Pin, kSszGpioOutOD);
	SszDateTime rtc;
	drv_rtc_get_time_all(&rtc);
	curr_time_secs = ssz_time_to_seconds(&rtc);
#else
	curr_time_secs = sim_rtc_get_seconds();
	SszDateTime rtc;
	ssz_seconds_to_time(curr_time_secs, &rtc);
#endif

#if LOW_POWER_UART_ENABLE
	ssz_traceln("curr time: %02d:%02d:%02d, passed: %ds", 
		rtc.hour, rtc.minute, rtc.second, curr_time_secs- g_sleep_last_check_rtc_time);
	g_sleep_last_check_rtc_time = curr_time_secs;
#endif
	return wake_up_time - curr_time_secs;
}

int app_sleep_check_rtc_to_get_rest_time(int32_t wake_up_time, int* next_check_rtc_secs) {
	int diff_secs = app_sleep_diff_secs_from_now_to(wake_up_time);

	//由于MCU的timer有误差,所以每一段时间需要检测一次RTC
	if (diff_secs >= SSZ_SEC_IN_MIN) {
		//剩余时间最大偏差>=每分钟最大偏差, 每差不多1分钟检测一次
		*next_check_rtc_secs = SSZ_SEC_IN_MIN - TIMER_DEVIATION_SECS_EACH_MINUTE;
	}
	else if (diff_secs >= (SSZ_SEC_IN_MIN/TIMER_DEVIATION_SECS_EACH_MINUTE) ) {
		//剩余时间最大偏差>=1秒, 计算下次检测RTC时间
		*next_check_rtc_secs = (SSZ_SEC_IN_MIN - TIMER_DEVIATION_SECS_EACH_MINUTE)*diff_secs / SSZ_SEC_IN_MIN;
	}
	else {
		//剩余时间的最大偏差<1秒, 不再检测RTC
		*next_check_rtc_secs = SSZ_SEC_IN_MIN;
	}
#if LOW_POWER_UART_ENABLE
	ssz_traceln("sleep rest: %ds", diff_secs);
#endif
	return diff_secs;
}
//check RTC time is arrived or over the wake up time
bool app_sleep_check_rtc_is_arrived(int32_t wake_up_time)
{
	if (app_sleep_diff_secs_from_now_to(wake_up_time)<=0) {
		return true;
	}
	else {
		return false;
	}

}
//return: true->wake by time out, false->wake by other
bool app_sleep_run_internal(int32_t wake_up_time)
{
	SystemMode last_mode = system_mode();
	set_system_mode(kSystemModeSleep);

	ssz_assert(!alarm_is_play_beep_several_times());
	g_sleep_is_need_exit = false;
	g_interrupt_is_confirm_key_pressed = false;
#if LOW_POWER_UART_ENABLE
	g_sleep_last_check_rtc_time = ssz_time_now_seconds();
#endif

	//计算剩余睡眠时间
	int rest_sleep_ms;
#if ENABLE_CHECK_RTC_TO_CORRECT_REST_SLEEP_TIME
	int	next_check_rtc_ms = 0;//下一次检测RTC的剩余休眠时间
	int	tmp = 0;
#endif

	if (wake_up_time > 0) {
#if ENABLE_CHECK_RTC_TO_CORRECT_REST_SLEEP_TIME
		rest_sleep_ms = 1000 * app_sleep_check_rtc_to_get_rest_time(wake_up_time, &tmp);
		next_check_rtc_ms = rest_sleep_ms - tmp * 1000;
#else
		rest_sleep_ms = 1000 * app_sleep_diff_secs_from_now_to(wake_up_time);
#endif
	}
	else {
		rest_sleep_ms = INT_MAX;
	}

	if(rest_sleep_ms>0){
		drv_lowpower_sleep_slaver();
	}
	
	int once_need_sleep_ms;
	int32_t tick_time_when_sleep = ssz_tick_time_now();
	bool is_wake_up_by_time_out;
	bool ret = true;

	//不停的睡眠醒来直到退出
	while (rest_sleep_ms>0) {
		//设置单次睡眠时间
		if (wake_up_time > 0) {
			once_need_sleep_ms = rest_sleep_ms;
			if (once_need_sleep_ms > WAKE_UP_INTERVAL_TIME_MS) {
				once_need_sleep_ms = WAKE_UP_INTERVAL_TIME_MS;
			}
		}
		else {
			once_need_sleep_ms = WAKE_UP_INTERVAL_TIME_MS;
		}

		//check if wake up key pressed
		if (g_interrupt_is_confirm_key_pressed) {
			ret = false;
			break;
		}

#ifdef SSZ_TARGET_MACHINE
		if(alarm_play_is_power_can_off()){
			is_wake_up_by_time_out = drv_lowpower_enter_lpsleep(once_need_sleep_ms, false);
		}else{
			is_wake_up_by_time_out = drv_lowpower_enter_lpsleep(once_need_sleep_ms, true);
			#ifdef SSZ_TARGET_MACHINE
			__HAL_RCC_GPIOE_CLK_ENABLE();//for led
			#endif
		}
#else
		is_wake_up_by_time_out = app_sleep_for_ms(once_need_sleep_ms);
#endif
		//将睡眠时的时间加到系统tick中
		ssz_tick_time_increase(once_need_sleep_ms);

#ifdef SSZ_TARGET_MACHINE
		HAL_RCC_DeInit();
		SystemCoreClockUpdate();
		HAL_InitTick (TICK_INT_PRIORITY);

#if LOW_POWER_UART_ENABLE
		ssz_gpio_enable_clock(GPIOA);
		MX_USART1_UART_Init();
#endif
#endif

		//check if wake up key pressed
		if (g_interrupt_is_confirm_key_pressed) {
			ret = false;
			break;
		}

		//handle the low power timer 
		timer_handle_all_of_group(TIMER_GROUP_LOW_POWER_MODE);
		if (g_sleep_is_need_exit) {
			ret = false;
			break;
		}

		//减少剩余睡眠时间
		if (wake_up_time > 0) {
			rest_sleep_ms -= ssz_tick_time_elapsed(tick_time_when_sleep);
			

#if ENABLE_CHECK_RTC_TO_CORRECT_REST_SLEEP_TIME
			//是否超过一定时间,超过一定时间检查一下RTC,重新确认剩余睡眠时间,
			//因为MCU自身的timer不精确,需要每隔一段时间检查一下
			if (next_check_rtc_ms >= rest_sleep_ms) {
				rest_sleep_ms = 1000 * app_sleep_check_rtc_to_get_rest_time(wake_up_time, &tmp);
				next_check_rtc_ms = rest_sleep_ms - tmp * 1000;
			}
#endif	
			tick_time_when_sleep = ssz_tick_time_now();
#if ENABLE_CHECK_RTC_TO_WAKE_UP
			//检测是否RTC已经到了
			if (app_sleep_check_rtc_is_arrived(wake_up_time)) {
				break;
			}
#endif
		}
	}

	//退出睡眠
#ifdef SSZ_TARGET_MACHINE
#if LOW_POWER_UART_ENABLE
	HAL_UART_DeInit(&PC_UART);
#endif
	drv_lowpower_exit_lpsleep();
#else
	sim_timer_set("systick", MSECS_PER_TICK, 1);
#endif
	drv_lowpower_wakeup_slaver();

	set_system_mode(last_mode);
	return ret;
}

void app_sleep_enter() {
	app_sleep_run_internal(-1);
}
//return: true->wake by time out, false->wake by other
bool app_sleep_enter_and_exit_at(SszDateTime * date_time) {
	int32_t secs;
	secs = ssz_time_to_seconds(date_time);
	return app_sleep_run_internal(secs);
}
//return: true->wake by time out, false->wake by other
bool app_sleep_enter_for_ms(int sleep_ms) {
	ssz_assert_fail();
	return false;
}

void app_sleep_exit() {
	g_sleep_is_need_exit = true;
}

