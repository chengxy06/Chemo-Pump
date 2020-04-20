#ifdef TEST
#include "greatest.h"
#include "drv_lowpower.h"
#include <stdio.h>
#include <stdlib.h>
#include "led.h"
#include "voice.h"
#include "infusion_motor.h"
#include "ui.h"
#include "mid_rtc.h"
#include "display.h"
#include "image_data.h"
#include "main_run.h"
#include "app_mcu_comm.h"
#include "sys_power.h"
#include "drv_rtc.h"
#define TEST_LOW_POWER_SLEEP  1 //1:master and slaver sleep, 2:slaver sleep
/***********************************************/
//this will call before every case at this suite
static void test_drv_lowpower_setup(void *arg) {
	(void)arg;
}
//this will call after every case at this suite
static void test_drv_lowpower_teardown(void *arg) {
	(void)arg;
}

/***********************************************/
static void test_drv_lowpower_enable() {
}

static void test_drv_lowpower_disable() {
}
static void slaver_enter_lpsleep(int data)
{
	sys_power_wake_up_slaver_pin_low();
	app_mcu_send_to_slave(COMM_SLAVER_MCU_SLEEP,0,0);
	g_drv_lowpower_is_slaver_wake_up = false;
	ssz_delay_ms(data);
}
static void slaver_exit_lpsleep()
{
	//ssz_delay_ms(100);
	ssz_traceln("low");
	sys_power_wake_up_slaver_pin_low();
	ssz_delay_ms(2);
	ssz_traceln("high");
	sys_power_wake_up_slaver_pin_high();
	//ssz_delay_ms(2);
	//sys_power_wake_up_slaver_pin_low();
}

static void test_drv_lowpower_enter_lpsleep() {
	SKIP();
	int i = 5;
	bool ret;
	int32_t curr_time;
	voice_play_low_once();
	//voice_play_high_cycle();
	//led_turn_on(kLedGreen);
	while (--i >= 0) {
		rtc_scan();
		if(i%2==0){
			//voice_play_low_cycle();
			led_turn_on(kLedYellow);		
		}else
		{
			//voice_play_high_cycle();
			led_turn_on(kLedRed);
		}
		ssz_traceln("RTC Time:%s, will sleep 1s", ssz_time_now_str());
	#if TEST_LOW_POWER_SLEEP==1
		drv_lowpower_sleep_slaver();
		ret =drv_lowpower_enter_lpsleep(1000, false);
		drv_lowpower_exit_lpsleep();
		drv_lowpower_wakeup_slaver();
		GASSERT(ret == true);
	#elif TEST_LOW_POWER_SLEEP==2
		slaver_enter_lpsleep(1000);
		slaver_exit_lpsleep();
	#endif
		rtc_scan();
		ssz_traceln("RTC Time:%s, wake up", ssz_time_now_str());

		//show image
		int x, y;
		const Image *img = get_image(kImgLogo);

		ui_set_bk_color(UI_BLACK);
		ui_clear();
		x = (DISPLAY_WIDTH - img->width) / 2;
		y = (DISPLAY_HEIGHT - img->height) / 2;
		ui_draw_img_at(x, y, img);
		ssz_delay_ms(1000);
		
		//wait wake up
		ssz_traceln("wait slaver wakeup");
		curr_time = ssz_tick_time_now();
		while (1)
		{
			//handle all
			if (main_handle_all() == false)
			{
				//if nothing need handle, go to sleep
				ssz_sleep();
			}
			if (g_drv_lowpower_is_slaver_wake_up){
				ssz_traceln("slaver wakeup OK");
				break;
			}
			if(ssz_tick_time_elapsed(curr_time)>=1000){
				ssz_traceln("slaver wakeup FAIL");
				break;
			}
		}
	}

//	printf("will sleep for ever, press confirm key to wakeup\n");
//	ret = drv_lowpower_enter_lpsleep(0, false);
//	drv_lowpower_exit_lpsleep();
//	GASSERT(ret == false);
//	return;
	printf("%s:turn on LED\n", ssz_time_now_str());
	led_turn_on(kLedRed);
	ssz_delay_ms(2000);
	led_turn_off();
	printf("%s:turn off LED\n", ssz_time_now_str());
	printf("test motor\n");
	infusion_motor_start(kBackward, -1, NULL);
	ssz_delay_ms(1000);
	infusion_motor_stop(0);
	printf("test alarm\n");
	voice_play_low_once();
	ssz_delay_ms(1000);

	printf("test OLED\n");
	int x, y;
	const Image *img = get_image(kImgLogo);

	ui_set_bk_color(UI_BLACK);
	ui_clear();
	x = (DISPLAY_WIDTH - img->width) / 2;
	y = (DISPLAY_HEIGHT - img->height) / 2;
	ui_draw_img_at(x, y, img);
	display_flush_dirty();
	ssz_delay_ms(1000);
}

static void test_drv_lowpower_exit_lpsleep() {
}

static void test_drv_lowpower_run_at_lpsleep() {
	drv_lowpower_sleep_slaver();

	ssz_traceln("will sleep 125ms");
	int32_t curr_time_secs;
	int once_need_sleep_ms = 125;
	drv_lowpower_enter_lpsleep(once_need_sleep_ms, false);
	ssz_tick_time_increase(once_need_sleep_ms);
#ifdef SSZ_TARGET_MACHINE
	HAL_RCC_DeInit();
	SystemCoreClockUpdate();
	HAL_InitTick (TICK_INT_PRIORITY);
	ssz_gpio_enable_clock(GPIOA);
	MX_USART1_UART_Init();
#endif
	ssz_traceln("wake");
	
	ssz_gpio_enable_clock(RTC_I2C_SCK_GPIO_Port);
	ssz_gpio_set_mode(RTC_I2C_SCK_GPIO_Port, RTC_I2C_SCK_Pin, kSszGpioOutOD);
	SszDateTime rtc;

	for(int i=0; i<10; i++){
		
		drv_rtc_get_time_all(&rtc);
		ssz_traceln("curr time: %02d:%02d:%02d", rtc.hour, rtc.minute, rtc.second);
		ssz_traceln("delay 60s");
		ssz_delay_ms(60000);
	}

	
	drv_lowpower_exit_lpsleep();
	drv_lowpower_wakeup_slaver();

}

/***********************************************/
#include "drv_lowpower_suite_def.h"
#endif


