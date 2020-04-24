/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-21 jlli
* Initial revision.
*
************************************************/
#include "infusion_motor.h"
#include "drv_infusion_motor.h"
#include "drv_stf_motor.h"
#include "msg.h"
#include "ssz_common.h"
#include "timer.h"
#include "event.h"
#include "alarm.h"
#include "mid_adc.h"
#include "app_mcu_comm.h"
#include "param.h"
#include "pressure_bubble_sensor.h"
#include "mid_common.h"
#include "app_delivery.h"

/************************************************
* Declaration
************************************************/
#define INFUSION_MOTOR_RUN_CHECK_TIME_MS 20
#define INFUSION_MOTOR_STUCK_TIME_MS 200
#define INFUSION_MOTOR_START_CHECK_CURRENT_TIME_MS 500
#define INFUSION_MOTOR_CURRENT_MAX 1000
#define INFUSION_MOTOR_VOLTAGE_MAX 700//500
#define INFUSION_MOTOR_ENCODER_ERROR_CHECK 10


/************************************************
* Variable 
************************************************/
int g_infusion_motor_encoder; // 编码器计数
int g_infusion_motor_optical_coupler_encoder; // 光耦计数
#if MOTOR_ENABLE_CHECK_ENCODER_AFTER_STOP
int g_infusion_motor_forward_encoder_after_stop;
int g_infusion_motor_backward_encoder_after_stop;
#endif
int g_infusion_motor_expect_optical_coupler_encoder; // 
int g_infusion_motor_expect_encoder;
int g_infusion_motor_stop_cause;
MotorStopHandler g_infusion_motor_stop_handler;
MotorState g_infusion_motor_state;
MotorDirection g_infusion_motor_running_dir;

//for monitor motor is run right
bool g_infusion_motor_is_event_set;
bool g_infusion_motor_is_stop_by_optical_coupler;
int g_infusion_motor_direction_error_count;
int  g_infusion_motor_a_b_direction_error_total;


int g_infusion_motor_last_encoder;
int g_infusion_motor_last_optical_coupler_encoder;
int32_t g_infusion_motor_check_last_encoder_time; 
int32_t g_infusion_motor_monitor_start_time;

int g_infusion_motor_error_counter_encoder;
int g_infusion_motor_error_counter_optical_encoder;
int g_infusion_motor_error_counter_direction;


#if MOTOR_ENABLE_CALC_SPEED
int g_infusion_motor_check_speed_encoder;
bool g_infusion_motor_is_speed_done=true;
int g_infusion_motor_check_speed_encoder_start_time;
int g_infusion_motor_check_speed_encoder_run_time;
float  g_infusion_motor_speed;
#endif

int g_infusion_motor_soft_run_pwm_level = 0;
// 输注电机电流过大次数
int g_infusion_motor_current_large_count = 0;
void infusion_motor_change_speed();
void infusion_motor_speed_change_start();
void infusion_motor_speed_change_stop();


int g_infusion_motor_run_start_pwm_level = 50;
int g_infusion_motor_pwm_step_num = 10 ;

//启动时判断是否可以运行
int g_infusion_motor_is_cannot_run;


/************************************************
* Function 
************************************************/
#if MOTOR_ENABLE_CHECK_ENCODER_AFTER_STOP
void infusion_motor_check_encoder_after_stop() {
	motor_printfln("run after stop: forward[%d],backward[%d]\n",
		g_infusion_motor_forward_encoder_after_stop,
		g_infusion_motor_backward_encoder_after_stop);
}
#endif

void infusion_motor_run_check() {
	//if the encoder is not changed, we think it is stuck, need stop motor
	if (ssz_tick_time_elapsed(g_infusion_motor_check_last_encoder_time) >= INFUSION_MOTOR_STUCK_TIME_MS) {
		if (g_infusion_motor_last_encoder == g_infusion_motor_encoder) {
            if (is_battery_low_when_motor_run()){
                set_battery_status_low_when_motor_run(false);
                msg_post_two_param(kMsgBatteryStatusWhenMotorRun, kBatteryEmpty, g_infusion_monitor_battery_empty_voltage_for_msg);
            } else {
                if (g_infusion_motor_error_counter_encoder < INFUSION_MOTOR_ENCODER_ERROR_CHECK) {
                    g_infusion_motor_error_counter_encoder++;
                    infusion_motor_stop(MOTOR_STOP_BY_FINISH_EXPECT_TARGET);
                } 
                
                if(g_infusion_motor_error_counter_encoder >= INFUSION_MOTOR_ENCODER_ERROR_CHECK) {
                    g_infusion_motor_error_counter_encoder = 0;
                    record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
                        kLogEventMotorStuck, 
                        ssz_tick_time_elapsed(g_infusion_motor_check_last_encoder_time), 
                        g_infusion_motor_encoder);
                    infusion_motor_stop(MOTOR_STOP_BY_STUCK);
                    alarm_set(kAlarmInfuMotorStuckID);
    			    motor_printfln("MOTOR_STOP_BY_STUCK\n");
                }
            }
		}
		else if (g_infusion_motor_last_optical_coupler_encoder == g_infusion_motor_optical_coupler_encoder)
		{
            if (is_battery_low_when_motor_run()){;
                set_battery_status_low_when_motor_run(false);
                msg_post_two_param(kMsgBatteryStatusWhenMotorRun, kBatteryEmpty, g_infusion_monitor_battery_empty_voltage_for_msg);
            } else {
                if (g_infusion_motor_error_counter_optical_encoder < INFUSION_MOTOR_ENCODER_ERROR_CHECK) {
                    g_infusion_motor_error_counter_optical_encoder++;
                    infusion_motor_stop(MOTOR_STOP_BY_FINISH_EXPECT_TARGET);
                } 
                
                if(g_infusion_motor_error_counter_optical_encoder >= INFUSION_MOTOR_ENCODER_ERROR_CHECK) {
                    g_infusion_motor_error_counter_optical_encoder = 0;
                    record_log_add_with_three_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
        				kLogEventOpticalCouplerError,
        				ssz_tick_time_elapsed(g_infusion_motor_check_last_encoder_time), 
        				g_infusion_motor_optical_coupler_encoder,g_infusion_motor_encoder);  
                    infusion_motor_stop(MOTOR_STOP_BY_OPTICAL_COUPLER_ERROR);
                    alarm_set(kAlarmOpticalCouplerErrorID);
    			    motor_printfln("MOTOR_STOP_BY_OPTICAL_COUPLER_ERROR\n");
                }
            }
		}
		else {
			g_infusion_motor_last_encoder = g_infusion_motor_encoder;
			g_infusion_motor_last_optical_coupler_encoder = g_infusion_motor_optical_coupler_encoder;
		}
		g_infusion_motor_check_last_encoder_time = ssz_tick_time_now();
	}
//	int current_ad = mid_adc_get_average_ADC_channel_value(kSZDJCurrentADC);
//	motor_printfln("motor current[%d]", current_ad);

	if (ssz_tick_time_elapsed(g_infusion_motor_monitor_start_time) >= INFUSION_MOTOR_START_CHECK_CURRENT_TIME_MS) {
		int current_ad = mid_adc_get_average_ADC_channel_value(kSZDJCurrentADC);
		int voltage_ad = M_get_voltage_from_adc(current_ad);
		motor_monitor_printfln("motor current:%d=%dmv", current_ad, voltage_ad);
		if (voltage_ad > INFUSION_MOTOR_VOLTAGE_MAX) {
			 g_infusion_motor_current_large_count++ ;
			 if(g_infusion_motor_current_large_count >= MOTOR_ALLOW_MOTOR_CURRENT_ERROR_COUNT ){
			 	motor_printfln("motor current:%d=%dmv too large,stop!", current_ad, voltage_ad);
				record_log_add_with_two_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
					kLogEventMotorCurrentTooLarge,
					voltage_ad,
					g_infusion_motor_encoder);
				infusion_motor_stop(MOTOR_STOP_BY_CURRENT_TOO_LARGE);
				alarm_set(kAlarmInfuMotorCurrentTooLargeID);//TODO:
			 }
			 else{
				 motor_printfln("motor current:%d=%dmv too large!", current_ad, voltage_ad);
			 }
		}
		else{
			g_infusion_motor_current_large_count = 0 ;
		}
	}
}
void infusion_motor_monitor_start() {
	g_infusion_motor_current_large_count = 0 ;
	g_infusion_motor_last_encoder = 0;
	g_infusion_motor_last_optical_coupler_encoder = 0;
	g_infusion_motor_check_last_encoder_time = ssz_tick_time_now();
	g_infusion_motor_monitor_start_time = ssz_tick_time_now();
	timer_start_periodic_every(kTimerInfusionMotorCheck, INFUSION_MOTOR_RUN_CHECK_TIME_MS);
}
void infusion_motor_monitor_stop() {
	timer_stop(kTimerInfusionMotorCheck);
}
//start infusion motor for the expected encoder in the set direction
bool infusion_motor_start_internal(MotorDirection dir, int expect_encoder,int expect_optical_coupler_encoder, MotorStopHandler stop_handler)
{
	pressure_and_bubble_sensor_pwr_enable();

	g_infusion_motor_a_b_direction_error_total = 0;
    g_infusion_motor_encoder = 0;
    g_infusion_motor_expect_encoder = expect_encoder;
	g_infusion_motor_optical_coupler_encoder = 0;
	g_infusion_motor_expect_optical_coupler_encoder = expect_optical_coupler_encoder;
    g_infusion_motor_running_dir = dir;
	g_infusion_motor_stop_cause = MOTOR_STOP_BY_USER;
	g_infusion_motor_stop_handler = stop_handler;

	g_infusion_motor_is_event_set = false;
	g_infusion_motor_direction_error_count = 0;

 
//	drv_infusion_motor_brake();  加上不同硬件下 expect_encoder数有差异

    app_mcu_send_to_slave(COMM_MOTOR_START,dir,0);
	ssz_delay_ms( 30 ); // 30

	if (g_infusion_motor_state == kMotorRun) {
		//need pause encoder count temporary for conflict
	}
	else {
		g_infusion_motor_is_cannot_run = false;
		msg_notify_int_at_once(kMSgBeforeInfusionMotorRun, expect_encoder);
		if (g_infusion_motor_is_cannot_run){
			app_mcu_send_to_slave(COMM_MOTOR_STOP,0,0); //disable infusion motor power at slave mcu 
			return false;
		}
		msg_post_int(kMSgInfusionMotorRun, expect_encoder);
	}
	g_infusion_motor_state=kMotorRun;
	//drv_all_motor_pwr_enable();

	int speed=-1;
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	if(dir == kForward){ 
		speed = g_infusion_motor_run_start_pwm_level;
	}
#endif 	
	
    if(dir == kBackward ){
		drv_infusion_motor_move_backward(speed);
		motor_printfln("motor start backward, expect:%d, expect optical:%d", expect_encoder, expect_optical_coupler_encoder);
    }else if(dir == kForward ){
		drv_infusion_motor_move_forward(speed);
		motor_printfln("motor start forward, expect:%d, expect optical:%d", expect_encoder, expect_optical_coupler_encoder);
    }else{
		ssz_assert_fail();
    }
	drv_infusion_motor_sleep_disable();

	infusion_motor_monitor_start();
#if	INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE == 1
	if(dir == kForward){ 
		infusion_motor_speed_change_start();
	}
#endif 

	return true;
}
//start infusion motor for the expected encoder in the set direction
bool infusion_motor_start(MotorDirection dir, int expect_encoder, MotorStopHandler stop_handler)
{
	return infusion_motor_start_internal(dir,expect_encoder,-1,stop_handler);
}
bool infusion_motor_start_ex(MotorDirection dir, int expect_encoder, MotorStopHandler stop_handler)
{
	return infusion_motor_start_internal(dir,expect_encoder,ONE_DROP_OPTICAL_COUPLER_ENCODER,stop_handler);
}


void infusion_motor_stop(int stop_cause)
{
    drv_infusion_motor_brake();
	if (g_infusion_motor_state == kMotorRun) {
		//wait some time to let it continue run by inertia
		ssz_delay_ms(50);
	}
    app_mcu_send_to_slave(COMM_MOTOR_STOP,0,0); //disable infusion motor power at slave mcu 
	
	drv_infusion_motor_sleep_enable();
	//drv_all_motor_pwr_disable();
	drv_infusion_motor_inertial_stop();
	infusion_motor_monitor_stop();
#if INFUSION_MOTOR_MOVE_FOR_PWM_ENABLE
	infusion_motor_speed_change_stop();
#endif

#if MOTOR_ENABLE_CHECK_ENCODER_AFTER_STOP
	g_infusion_motor_forward_encoder_after_stop = 0;
	g_infusion_motor_backward_encoder_after_stop = 0;
	timer_set_handler(kTimerCheckEncoderAfterMotorStop, infusion_motor_check_encoder_after_stop);
	timer_start_oneshot_after(kTimerCheckEncoderAfterMotorStop, 3000);
#endif

	if (g_infusion_motor_state != kMotorStop) {
		g_infusion_motor_state = kMotorStop;
		motor_printfln("motor stop, have run:%d, expect:%d, cause:%d",
		 g_infusion_motor_encoder, g_infusion_motor_expect_encoder,stop_cause);
		motor_printfln("optical_coupler have run:%d, expect:%d",
		 g_infusion_motor_optical_coupler_encoder,g_infusion_motor_expect_optical_coupler_encoder);
		motor_printfln("error total have run:%d ",
		 g_infusion_motor_a_b_direction_error_total );
		g_infusion_motor_a_b_direction_error_total = 0;

		msg_post_int(kMSgInfusionMotorStop, g_infusion_motor_encoder);
		if (g_infusion_motor_stop_handler) {
			g_infusion_motor_stop_handler(g_infusion_motor_encoder, stop_cause);
		}

	}

}
int infusion_motor_encoder_after_start()
{
	return g_infusion_motor_encoder;
}

int infusion_motor_coupler_encoder_after_start()
{
	return g_infusion_motor_optical_coupler_encoder;
}

int infusion_motor_expect_encoder()
{
	return g_infusion_motor_expect_encoder;
}
MotorDirection infusion_motor_direction()
{
    return g_infusion_motor_running_dir;
}

bool infusion_motor_is_running()
{
    bool ret;
    if(g_infusion_motor_state == kMotorRun){
        ret = true;
    }else{
        ret = false;
    }
    return ret;
}

int infusion_motor_stop_cause()
{
	return g_infusion_motor_stop_cause;
}


void infusion_motor_on_finish_target() {
	infusion_motor_stop(MOTOR_STOP_BY_FINISH_EXPECT_TARGET);
}
void infusion_motor_on_direction_wrong() {
    if (is_battery_low_when_motor_run()){
        set_battery_status_low_when_motor_run(false);
        msg_post_two_param(kMsgBatteryStatusWhenMotorRun, kBatteryEmpty, g_infusion_monitor_battery_empty_voltage_for_msg);
    } else {

        if (g_infusion_motor_error_counter_direction < INFUSION_MOTOR_ENCODER_ERROR_CHECK) {
            g_infusion_motor_error_counter_direction++;
            infusion_motor_stop(MOTOR_STOP_BY_FINISH_EXPECT_TARGET);
        }
        
        if(g_infusion_motor_error_counter_direction >= INFUSION_MOTOR_ENCODER_ERROR_CHECK) {
            g_infusion_motor_error_counter_direction = 0;
            record_log_add_with_one_int(kOperationLog, LOG_EVENT_TYPE_ERROR,
                kLogEventMotorDirectionError,
                g_infusion_motor_encoder);
            infusion_motor_stop(MOTOR_STOP_BY_DIRECTION_WRONG);
            alarm_set(kAlarmInfuMotorDirectionErrorID);
            motor_printfln("MOTOR_STOP_BY_DIRECTION_ERROR\n");
        }
    }
}
#if MOTOR_ENABLE_CALC_SPEED
void infusion_motor_get_speed_done() 
{
    g_infusion_motor_speed=MOTOR_SPEED_CHECK_COUNT*1000.0/g_infusion_motor_check_speed_encoder_run_time;

    motor_printfln("motor speed:%.2f[Hz], speed-check run time:%d[ms]",
     g_infusion_motor_speed, g_infusion_motor_check_speed_encoder_run_time);
}
#endif
void infusion_motor_init()
{
	timer_set_handler(kTimerInfusionMotorCheck, infusion_motor_run_check);
	event_set_handler(kEventInfusionMotorFinishTarget, infusion_motor_on_finish_target);
	event_set_handler(kEventInfusionMotorDirectionWrong, infusion_motor_on_direction_wrong);
#if MOTOR_ENABLE_CALC_SPEED
    event_set_handler(kEventInfusionMotorGetSpeedFinished, infusion_motor_get_speed_done);
#endif
}

void infusion_motor_speed_change_start()
{
	g_infusion_motor_soft_run_pwm_level = g_infusion_motor_run_start_pwm_level ;
	timer_set_handler(kTimerInfusionMotorSoftRun, infusion_motor_change_speed);
	timer_start_periodic_every(kTimerInfusionMotorSoftRun, INFUSION_MOTOR_START_PWM_RUN_TIME_MS);
} 

void infusion_motor_change_speed()
{	
	g_infusion_motor_soft_run_pwm_level = g_infusion_motor_soft_run_pwm_level + g_infusion_motor_pwm_step_num ;
    if (g_infusion_motor_soft_run_pwm_level >= INFUSION_MOTOR_START_PWM_MAX_LEVEL){
        g_infusion_motor_soft_run_pwm_level = INFUSION_MOTOR_START_PWM_MAX_LEVEL;
    }
    drv_infusion_motor_change_duty_cycle(g_infusion_motor_soft_run_pwm_level);
    if(g_infusion_motor_soft_run_pwm_level >= INFUSION_MOTOR_START_PWM_MAX_LEVEL){	
		if(g_infusion_motor_running_dir == kForward ){
			ssz_traceln("change to full speed");
			//drv_infusion_motor_move_forward_ex();
		}
		timer_stop(kTimerInfusionMotorSoftRun);
	}
}
void infusion_motor_speed_change_stop()
{
	timer_stop(kTimerInfusionMotorSoftRun);
}


