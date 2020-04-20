/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-15 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "sys_power.h"

/************************************************
* Declaration
************************************************/


//all system's state
typedef enum
{
	kSystemStateWaitPowerUp,
	kSystemStatePost,
	kSystemStatePowerOn,
	kSystemStatePowerOff
}SystemState;

//all system's mode
typedef enum
{
	kSystemModeSleep,
	kSystemModeNormal,
	kSystemModeFactory,
	kSystemModeEngineer,
}SystemMode;

#ifdef __cplusplus
extern "C" {
#endif
//current system's state
SystemState system_state(void);

//set current state
void set_system_state(SystemState state);

//current system's mode
SystemMode system_mode(void);

//set current mode
void set_system_mode(SystemMode mode);

//is system ready(is finish init and post)
bool is_system_ready(void);

//set system as ready(when finish init and post, need set it)
void set_system_ready(void);

//get current battery level
BatteryLevel battery_level(void);

//battery is charging
bool is_battery_charging(void);

//is AC connected
bool is_use_extern_power(void);

//is battery exist
bool is_battery_exist(void);

//is pill box installed
bool is_pill_box_install();

//is get the pressure sensor adc when install bag
bool is_pressure_adc_ready_after_installed();

//the pressure sensor adc when install bag
int pressure_adc_when_installed();
bool is_battery_low_when_motor_run(void);
void set_battery_status_low_when_motor_run(bool status);
bool is_battery_empty_when_motor_run(void);
void set_battery_status_empty_when_motor_run(bool status);

#ifdef __cplusplus
}
#endif

