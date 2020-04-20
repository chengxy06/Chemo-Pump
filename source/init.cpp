#include "init.h"
#include "ssz_common.h"
#include "dev_def.h"

//resource and lib
#include "ssz_locale.h"
#include "font_data.h"
#include "string_data.h"
#include "ui_view.h"  
#include "simple_alloc.h"
#include "common.h"

//driver
#include "drv_com.h"
#include "drv_sst25_flash.h"
#include "drv_isd2360.h"
#include "drv_rtc.h"
#include "drv_oled.h"
#include "drv_lowpower.h"
#include "drv_stf_motor.h"
#include "drv_infusion_motor.h"

//middleware
#include "data.h"
#include "record_log.h"
#include "event.h"
#include "msg.h"
#include "key.h"
#include "com.h"
#include "alarm.h"
#include "screen.h"
#include "display.h"
#include "mid_adc.h"
#include "pill_box_install_detect.h"
#include "infusion_motor.h"
#include "three_valve_motor.h"
#include "vibrater_motor.h"
#include "mid_rtc.h"
#include "mid_common.h"
#include "led.h"
#include "stop_watch.h"
#include "param.h"

//app
#include "app_system_monitor.h"
#include "app_motor_monitor.h"
#include "app_infusion_monitor.h"
#include "app_delivery.h"
#include "app_scr_event_handle.h"
#include "app_key_event_handle.h"
#include "app_alarm_event_handle.h"
#include "app_infusion_event_handle.h"
#include "app_msg_handle.h"
#include "app_cmd.h"
#include "app_mcu_comm.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
void init_common() {
#ifdef DEBUG
	set_module_output(kModuleCommon, true);
	set_module_output(kModuleMotor, true);
	set_module_output(kModuleScreen, true);
	//set_module_output(kModuleInfusionMonitor, true);
#else
#if STOP_WATCH_MODULE_ENABLE
	stop_watch_disable();
#endif
#endif

	//init simple alloc
	salloc_config();

	//init system language and country region
	ssz_locale_set_lang(kSszLangSimplifiedChinese);
	ssz_locale_set_country_region(kSszCountryRegionChina);
#if 1//UI_VIEW_MODULE_ENABLE
	//init resource
	font_data_select_by_lang(ssz_locale_lang());
	string_data_select_by_lang(ssz_locale_lang());
	//config ui
	ui_config();
	//config ui view
	ui_view_config();

	ui_view_init();
#endif
}
void init_driver(){

#if STEP_DEBUG_ENABLE
	//for test
	sys_power_enable();
#endif

	dev_init();
	drv_com_init();
	ssz_traceln("init driver");
	//enable common power
	drv_all_motor_pwr_enable();
	drv_all_5v_pwr_enable();
	mid_adc_vref_enable();
//printf("dev_init finish\n");		

//printf("drv_com_init \n");		
	drv_rtc_init();
//printf("drv_rtc_init \n");		
	//drv_oled_init();
//printf("drv_oled_init \n");	
	drv_sst25_flash_init();
//printf("drv_sst25_flash_init \n");		
	//drv_isd2360_initial();
	drv_isd2360_deinitial();
//printf("drv_isd2360_initial \n");		
	drv_infusion_motor_init();
	drv_three_valve_motor_init();
}

void init_middleware(){
	ssz_traceln("init middleware");

	msg_init();
	alarm_init();
	rtc_init_time_if_not_set(NULL);
	stop_watch_init();
	mid_adc_init();	
	led_init();
	pill_box_install_sensor_init();
	infusion_motor_init();
	three_valve_motor_init();
	vibrator_motor_init();

	data_init();
	record_log_init();
	key_init();
	//init com
	com_init_by_file(kComPCUart, drv_com_file(kComPCUart));
	com_init_by_file(kComMCUUart, drv_com_file(kComMCUUart));
	com_init_by_file(kComBTUart, drv_com_file(kComBTUart));
	display_init();

    int precision_factor = data_read_int(kDataPrecisionFactor);
    if (precision_factor < 50 || precision_factor > 150) {
        precision_factor = 100;
        data_write_int(kDataPrecisionFactor, precision_factor);
    }
    //read encoder counter of one ml
    int encoder_counter = data_read_int(kDataEncoderCountEachML);
	if (encoder_counter>0) {
		set_encoder_count_of_one_ml((int)(encoder_counter * precision_factor / 100.0));
	}

    //read battery offset ad
	int offset = data_read_int(kDataBatteryRedress);
	set_battery_offset_ad(offset);
    //read bubble alarm setting configuration
    bool bubble_alarm = data_read_int(kDataBubbleAlarmSwitch);
    set_bubble_alarm_status(bubble_alarm);
    set_pressure_threshold(data_read_int(kDataPressureThreshold));
    
}

void init_app(){
	ssz_traceln("init app");

	screen_init(kScreenHome);
	app_cmd_init();
	app_motor_monitor_init();
	app_mcu_comm_init();
	
	app_scr_event_handle_init();
	app_key_event_handle_init();
	app_alarm_event_handle_init();
	app_infusion_event_handle_init();
	app_msg_handle_init();
	app_delivery_init();
	app_infusion_monitor_init();
}

void init_all(){
	init_common();
	init_driver();
	init_middleware();
	init_app();
}
