/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-31 dczhang
* Initial revision.
*
************************************************/
#include "app_startup_and_post.h"
#include "ui.h"
#include "image_data.h"
#include "main.h"
#include "ssz_gpio.h"
#include "sys_power.h"
#include "ssz_tick_time.h"
#include "ssz_config.h"
#include "alarm.h"
#include "key.h"
#include "led.h"
#include "infusion_motor.h"
#include "drv_key.h"
#include "display.h"
#include "pill_box_install_detect.h"
#include "param.h"
#include "voice.h"
#include "app_mcu_comm.h"
#include "pressure_bubble_sensor.h"
#include "common.h"
#include "drv_stf_motor.h"
#include "three_valve_motor.h"


/************************************************
* Declaration
************************************************/
#define DELYA_MS_TO_SHOW_LED 300 //1000 

/************************************************
* Variable 
************************************************/


/************************************************
* Function 
************************************************/

static void app_post_display_check()
{
    int x,y;
    const Image *img = get_image(kImgLogo);
	
	ui_set_bk_color(UI_BLACK);
	ui_clear();
	x = (DISPLAY_WIDTH - img->width) / 2;
	y = (DISPLAY_HEIGHT - img->height) / 2;
	ui_draw_img_at(x, y, img);
	display_flush_dirty();
	//ssz_delay_ms(3000);

}

static void app_post_judge_key_stuck()
{
    int8_t key_states[kKeyIDMax];

	drv_key_scan(key_states);   
    //跳过keypower
    for(int i = 1; i < kKeyIDMax; i++){
        if(i!=kKeyBack && key_states[i] == true){
			record_log_add_with_one_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
				kLogEventKeyStuck,
				i);
            alarm_set(kAlarmKeyStuckID);
            break;
        }
    }
}

static void app_post_led_red_check()
{
    led_turn_on(kLedRed);
    ssz_delay_ms(DELYA_MS_TO_SHOW_LED);
}

static void app_post_led_yellow_check()
{
    led_turn_on(kLedYellow);
    ssz_delay_ms(DELYA_MS_TO_SHOW_LED);
}

static void app_post_led_green_check()
{
    led_turn_on(kLedGreen);
    ssz_delay_ms(DELYA_MS_TO_SHOW_LED);
}

static void app_post_beep_and_voice_check()
{
    app_mcu_send_to_slave(COMM_BEEP_TEST,0,0);
    ssz_delay_ms(DELYA_MS_TO_SHOW_LED);
    voice_play_beep_once();
    ssz_delay_ms(300);
    voice_play_beep_once();
}

//return: true->test running, false->can not test
static bool app_post_infusion_motor_check()
{
	if (!pill_box_install_left_detect() && !pill_box_install_right_detect()) {
        infusion_motor_start(kBackward, 400, NULL);
		return true;
	}else{
		return false;
	}
}

static void app_post_three_valve_motor_pos_check()
{
    bool is_three_valve_right = true;

    if (drv_three_valve_is_pos_b_detected()) {
        ;
    }
    else {
        is_three_valve_right=three_valve_motor_move_to_position_b();
    }

    if (!is_three_valve_right) {
		//alarm_set(kAlarmSTFMotorErrorID);
	}
}

static void app_post_bubble_sensor_check()
{
	if (!pill_box_install_left_detect() && !pill_box_install_right_detect()) {
        if(!pressure_bubble_sensor_is_generate_bubble()){
            alarm_set(kAlarmBubbleSensorErrorID);
        }
	}
}

static void app_post_pressure_sensor_check()
{
    if (!pill_box_install_left_detect() && !pill_box_install_right_detect()) {
        if(M_get_voltage_from_adc(pressure_bubble_sensor_get_pressure_sensor_ADC_average_value()) > 200){
			record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
				kLogEventPressureSensorError,
				true,
				pressure_bubble_sensor_get_pressure_sensor_ADC_average_value());
            alarm_set(kAlarmPressureSensorErrorID);
        }
	}
//    if (pill_box_install_left_detect() && pill_box_install_right_detect()) {
//        if(M_get_voltage_from_adc(pressure_bubble_sensor_get_pressure_sensor_ADC_average_value()) < 500)
//			record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
//				kLogEventPressureSensorError,
//				false,
//				pressure_bubble_sensor_get_pressure_sensor_ADC_average_value());
//            alarm_set(kAlarmPressureSensorErrorID);
//    }
}

void app_post()
{
	bool is_need_check_motor = false;

	is_need_check_motor = app_post_infusion_motor_check();
	app_post_display_check();

	app_post_led_red_check();
    app_post_led_yellow_check();
    app_post_led_green_check();
	led_turn_off();
    //power on self test
    app_post_beep_and_voice_check();
    //app_post_three_valve_motor_pos_check();
    app_post_bubble_sensor_check();
    app_post_pressure_sensor_check();
	if (is_need_check_motor) {
		infusion_motor_stop(0);
		if (g_infusion_motor_encoder <= 0) {
			alarm_set(kAlarmInfuMotorStuckID);
		}
	}
}
void app_wait_key_press_to_start_up()
{
    int cur_time = 0;
    int expire_time = 0;

    //power up if the power key hold 1s
    for(int i = 0; i < KEY_HOLD_TIME_MS_TO_POWER_UP/5; i++){
        expire_time = ssz_tick_time_next(5);
        cur_time = ssz_tick_time_now();
        //wait reach to expire time
        while(cur_time < expire_time){
            cur_time = ssz_tick_time_now();
        }
#ifdef SSZ_TARGET_SIMULATOR
		break;
#endif
        //disable the power pin if power down
        if(!drv_key_is_pressed(kKeyBack)){
            app_mcu_send_to_slave(COMM_POWER_OFF,0,0);
            sys_power_disable();
        }
    }

}







