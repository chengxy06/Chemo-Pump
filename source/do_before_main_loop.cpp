#include "config.h"
#include "init.h"
#include "app_home.h"
#include "mid_rtc.h"
#include "key.h"
#include "app_system_monitor.h"
#include "app_startup_and_post.h"
#include "app_main_menu.h"
#include "app_check_last_infuse.h"
#include "app_mcu_comm.h"
#include "alarm.h"
#include "ui.h"
#include "app_home.h"
#include "ui_view.h"
#include "ui_common.h"
#include "record_log.h"
#include "param.h"
#include "dtimer.h"
#include "drv_stf_motor.h"
#include "pill_box_install_detect.h"
#include "sys_power.h"
#include "drv_lowpower.h"
#include "mid_common.h"
#include "app_mcu_monitor.h"
#include "app_common.h"

/************************************************
* Declaration
************************************************/


/************************************************
* Variable 
************************************************/


/************************************************
* Function 
************************************************/
void start_system_scan() {
	//start scan dtimer
#if DTIMER_MODULE_ENABLE
	dtimer_start_scan_timer();
#endif
	//start scan ui timer
	ui_view_start_scan_timer();
	//start scan rtc
	rtc_start_scan();
	//start scan key
	key_start_scan();
	//start monitor system
	app_system_monitor_start();
}
void stop_system_scan() {
	//stop scan dtimer
#if DTIMER_MODULE_ENABLE
	dtimer_stop_scan_timer();
#endif
	//stop scan ui timer
	ui_view_stop_scan_timer();
	//stop scan rtc
	rtc_stop_scan();
	//stop scan key
	key_stop_scan();
	//stop monitor system
	app_system_monitor_stop();
}

extern "C" void do_before_main_loop() {
	set_system_mode(kSystemModeNormal);
	//below power need enable for system run
	drv_three_valve_pos_detect_pwr_enable();
	pill_box_install_sensor_left_pwr_enable();
	pill_box_install_sensor_right_pwr_enable();
	sys_power_enable();
	sys_master_mcu_power_enable();
	sys_power_wake_up_slaver_pin_low();
	//wait three valve pos detec enable
	ssz_delay_us(100);
	//wakeup slaver
	sys_power_wake_up_slaver_pin_high();
	ssz_delay_us(100);
#ifdef SSZ_TARGET_MACHINE
 	int RCC_CSR_register = RCC->CSR;
	// 正常为复位 RCC_CSR_register  		0XC000000
	ssz_traceln("RCC_CSR register = 0X%X", RCC_CSR_register);
    
	if(RCC_CSR_register != 0XC000000)
	{  // 异常保存寄存器数值		
		LogVar log_var[1];
	    log_var[0].type = kLogVarInteger;
	    log_var[0].value = RCC_CSR_register;
	    record_log_add(kOperationLog, LOG_EVENT_TYPE_INFO, 
	        kLogEventMachineRestartArmCSR,log_var, ssz_array_size(log_var));
	}
#endif

	//check if reset by slave, if reset by slave, skip post and power key
	//S_RST_STA_Pin high reset by slave
	 if(ssz_gpio_is_high(S_RST_STA_GPIO_Port, S_RST_STA_Pin) == false){ 
#if !STEP_DEBUG_ENABLE
		//press power key to power on
		app_wait_key_press_to_start_up();
#endif
		app_post();
		//because user need hold power key to power on, so set power key as hold
		key_set_hold_time(kKeyBack, KEY_HOLD_TIME_MS);
	} else {
		//reset by slave
		alarm_set(kAlarmMasterMCUErrorID);
		ssz_traceln("reset by slave");		
		record_log_add_with_data(kOperationLog, LOG_EVENT_TYPE_ALARM,
			kLogEventMasterMcuResetByslaver, NULL, 0);		
	}

	record_log_add_info_with_data(kHistoryLog, kLogEventPowerOn, NULL, 0);

	//start scan system 
	start_system_scan();

	//create statusbar
	statusbar_create();

	//enter home screen
	app_home_enter();

	//check if need resume infuse or create new infuse
	app_check_last_infuse();

	//notify smcu 
	app_mcu_send_to_slave(COMM_POWER_ON, 0, 0);
#if STEP_DEBUG_ENABLE
	watchdog_stop();
#else
	watchdog_start();
#endif	
//	drv_isd2360_mcu_io_as_analog();
}

