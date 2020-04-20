/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-07-10 jlli
* Initial revision.
*
************************************************/
#include "app_motor_monitor.h"
#include "infusion_motor.h"
#include "three_valve_motor.h"
#include "alarm.h"
#include "msg.h"
#include "app_delivery.h"
#include "data.h"
#include "pressure_bubble_sensor.h"
#include "timer.h"
#include "ssz_common.h"

/************************************************
* Declaration
************************************************/
#define MOTOR_MONITOR_CHECK_TIME_MS 20
/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/


void app_motor_monitor_check()
{
}

void app_motor_monitor_start()
{
    timer_set_handler(kTimerInfusionMotorMonitor, app_motor_monitor_check);
    timer_start_periodic_every(kTimerInfusionMotorMonitor, MOTOR_MONITOR_CHECK_TIME_MS);
}

void app_motor_monitor_stop()
{
    timer_stop(kTimerInfusionMotorMonitor);
}

void app_motor_monitor_handle_msg_start(MsgParam param)
{
    
}

void app_motor_monitor_handle_msg_stop(MsgParam param)
{
    
}

void app_motor_monitor_init()
{
    msg_set_handler(kMSgInfusionMotorRun, app_motor_monitor_handle_msg_start);
    msg_set_handler(kMSgInfusionMotorStop, app_motor_monitor_handle_msg_stop);
}

