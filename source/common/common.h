#pragma once

#include "ssz_def.h"
#include "ssz_common.h"
#include "config.h"

/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif


//adc refer voltage(mV)
#define ADC_REF_VOLTAGE 3300 
//adc max digit code = 2^(adc's bits)
#define ADC_MAX_DIGIT_CODE 4095

//get voltage(mV) from ADC value, the voltage's unit is mV
#define M_get_voltage_from_adc(adc_value) ((adc_value)*ADC_REF_VOLTAGE/ADC_MAX_DIGIT_CODE)

//get ADC value from voltage(mV)
#define M_get_adc_from_voltage(voltage_value) ((voltage_value)*ADC_MAX_DIGIT_CODE/ADC_REF_VOLTAGE)

	
//voltage dadio(real battery voltage/volatge get from mcu)
#define BATTERY_VOLTAGE_RADIO 2

//voltage dadio(real battery voltage/volatge get from mcu)
#define MASTER_MCU_VOLTAGE_RADIO 2

//get real battery voltage(mV) from ADC value
#define M_get_battery_voltage_from_adc(adc_value) ((adc_value)*ADC_REF_VOLTAGE*BATTERY_VOLTAGE_RADIO /ADC_MAX_DIGIT_CODE)
	
//get battery ADC value from real battery voltage(mV)
#define M_get_adc_from_battery_voltage(voltage_value) ((voltage_value)*ADC_MAX_DIGIT_CODE/(BATTERY_VOLTAGE_RADIO*ADC_REF_VOLTAGE))

//get real slaver mcu voltage(mV) from ADC value
#define M_get_slaver_mcu_voltage_from_adc(adc_value) ((adc_value)*ADC_REF_VOLTAGE*MASTER_MCU_VOLTAGE_RADIO /ADC_MAX_DIGIT_CODE)
			
//get slaver mcu ADC value from real battery voltage(mV)
#define M_get_adc_from_slaver_mcu_voltage(voltage_value) ((voltage_value)*ADC_MAX_DIGIT_CODE/(MASTER_MCU_VOLTAGE_RADIO*ADC_REF_VOLTAGE))
	

//#define common_printfln(...) if(ssz_is_module_enable_output(kModuleCommon)){printf("[%d]:[common]:",ssz_tick_time_now());printf(__VA_ARGS__);printf("\n");}
#define common_printfln(...) ssz_printfln_ex(is_module_enable_output(kModuleCommon),COMMON_MODULE_NAME,__VA_ARGS__)


//define modules used to filter output
typedef enum
{
	kModuleCommon,
	kModuleMotor,
	kModuleMotorMonitor,
	kModuleInfusionMonitor,
	kModuleScreen,
	kModuleIDMax
}ModuleID;

#define COMMON_MODULE_NAME "common"
#define INFUSION_MONITOR_MODULE_NAME "infusion_monitor"
#define MOTOR_MODULE_NAME "motor"
#define MOTOR_MONITOR_MODULE_NAME "motor_monitor"
#define SCREEN_MODULE_NAME "screen"

typedef struct {
	const char* module_name;
	bool is_enable_output;
}ModuleInfo;

//set the module output enable/disable
void set_module_output(ModuleID module, bool is_enable);
//return is the module enable output
bool is_module_enable_output(ModuleID module);
//return module name
const char* get_module_name(ModuleID module);

extern bool g_common_is_enable_output;
#ifdef __cplusplus
}
#endif

