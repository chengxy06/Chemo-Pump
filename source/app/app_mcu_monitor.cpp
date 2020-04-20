/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2018-04-25 xqzhao
* Initial revision.
*
************************************************/
#include "app_mcu_monitor.h"
#include "timer.h"
#include "ssz_common.h"
#include "ssz_tick_time.h"
#include "app_mcu_comm.h"
#include "param.h"
#include "common.h"
#include "app_common.h"

/************************************************
* Declaration
************************************************/
#define MCU_ALL_RESET_MAX_COUNT 3
#define RESET_DELAY_MS  200

/************************************************
* Variable 
************************************************/
static int g_mcu_monitor_reset_count = 0;
static int g_mcu_monitor_watchdog_expired_time = WATCHDOG_EXPIRED_TIME_MS;

/************************************************
* Function 
************************************************/
void app_mcu_monitor_set_slave_reset_pin_high()
{
	ssz_gpio_set(SLAVER_NRST_GPIO_Port, SLAVER_NRST_Pin);
	ssz_delay_ms(5);
	//让slaver开机
	app_mcu_send_to_slave(COMM_POWER_ON, 0, 0);
	//启动watchdog
	watchdog_start();
}

void app_mcu_monitor_reset_slave_on_time_expired()
{
	g_mcu_monitor_reset_count++;
	if (g_mcu_monitor_reset_count> MCU_ALL_RESET_MAX_COUNT) {
		common_printfln("slave not active and reset too much times, alarm user");
		alarm_set(kAlarmSlaverMCUErrorID);
		watchdog_stop();
		return;
	}
	common_printfln("slave not active, reset it %d times", g_mcu_monitor_reset_count);
	record_log_add_with_one_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
		kLogEventSlaverNotActive, g_mcu_monitor_reset_count);
	ssz_gpio_clear(SLAVER_NRST_GPIO_Port, SLAVER_NRST_Pin);
	watchdog_stop();
	timer_start_oneshot_after(kTimerResetDelay, RESET_DELAY_MS);
	timer_start_oneshot_after(kTimerWatchdog, WAIT_OPPOSITE_MCU_POWER_ON_TIME_MS+g_mcu_monitor_watchdog_expired_time);
}
void app_mcu_monitor_start(){
#ifdef SSZ_TARGET_MACHINE
	if (timer_is_actived(kTimerWatchdog)) {
		return;
	}
	timer_set_handler(kTimerWatchdog, app_mcu_monitor_reset_slave_on_time_expired);
	timer_set_handler(kTimerResetDelay, app_mcu_monitor_set_slave_reset_pin_high);
	timer_start_oneshot_after(kTimerWatchdog, g_mcu_monitor_watchdog_expired_time);
#endif
}
void app_mcu_monitor_stop() {
	timer_stop(kTimerWatchdog);
}
void app_mcu_monitor_stop_and_clear_reset_count() {
	g_mcu_monitor_reset_count = 0;
	app_mcu_monitor_stop();
}
void app_mcu_monitor_set_watchdog_expired_time(int expired_time){
	g_mcu_monitor_watchdog_expired_time = expired_time;
	if (timer_is_actived(kTimerWatchdog)) {
		timer_start_oneshot_after(kTimerWatchdog, g_mcu_monitor_watchdog_expired_time);
	}	
}

