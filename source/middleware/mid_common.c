/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-15 xqzhao
* Initial revision.
*
************************************************/
#include "mid_common.h"
#include "param_internal.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static SystemState g_system_state = kSystemStatePowerOn;
static SystemMode g_system_mode = kSystemModeNormal;
//is finish init and post
static bool g_system_is_ready = false;
static bool g_battery_is_low = false;
static bool g_battery_is_empty = false;



/************************************************
* Function 
************************************************/



/***********************************************
* Description:
*   get current system state
* Argument:
*
* Return:
*   current system state
************************************************/
SystemState system_state(void)
{
	return g_system_state;
}

/***********************************************
* Description:
*   set system state
* Argument:
*   state:
*
* Return:
*
************************************************/
void set_system_state(SystemState state)
{
	if (state != g_system_state)
	{
		g_system_state = state;
	}
}

/***********************************************
* Description:
*   get current mode
* Argument:
*
* Return:
*   current mode
************************************************/
SystemMode system_mode(void)
{
	return g_system_mode;
}

/***********************************************
* Description:
*   set current mode
* Argument:
*   mode:
*
* Return:
*
************************************************/
void set_system_mode(SystemMode mode)
{
	g_system_mode = mode;
}

/***********************************************
* Description:
*   is system ready(is finish init and post)
* Argument:
*
* Return:
*
************************************************/
bool is_system_ready(void)
{
	return g_system_is_ready;
}

/***********************************************
* Description:
*   set system as ready(when finish init and post, need set it)
* Argument:
*
* Return:
*
************************************************/
void set_system_ready(void)
{
	g_system_is_ready = true;
}

//get current battery level
BatteryLevel battery_level(void)
{
	return g_current_battery_level;
}

//battery is charging
bool is_battery_charging(void)
{
	return false;
}

//is AC connected
bool is_use_extern_power(void)
{
	return g_current_is_use_extern_power;
}

//is battery exist
bool is_battery_exist(void)
{
	if (g_current_battery_level==kBatteryNotExist) {
		return false;
	}
	else {
		return true;
	}
}

//is pill box installed
bool is_pill_box_install() {
	return g_is_pill_box_installed;
}

//is get the pressure sensor adc when install bag
bool is_pressure_adc_ready_after_installed(){
	return g_is_pressure_adc_ready_when_installed;
}

//the pressure sensor adc when install bag
int pressure_adc_when_installed() {
	return g_pressure_adc_when_installed;
}

bool is_battery_low_when_motor_run(void)
{
    return g_battery_is_low;
}

void set_battery_status_low_when_motor_run(bool status)
{
    g_battery_is_low = status;
}

bool is_battery_empty_when_motor_run(void)
{
    return g_battery_is_empty;
}

void set_battery_status_empty_when_motor_run(bool status)
{
    g_battery_is_empty = status;
}


