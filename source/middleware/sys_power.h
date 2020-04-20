/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-20 dczhang
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "ssz_gpio.h"
#include "mid_adc.h"
#include "msg.h"



#define SYS_PWR_EN_PIN SYS_PWR_EN_Pin
#define SYS_PWR_ADAPTER_INSERT_PIN AC_CONNECT_Pin      // 高电平，则外部适配器插入
#define SYS_PWR_ADC_EN_PIN SYS_PWR_ADC_EN_Pin
#define SYS_PWR_COM_PORT SYS_PWR_EN_GPIO_Port

//根据电池特性曲线更改
#define BATT_LEVEL_NOT_EXIST 650
#define BATT_LEVEL_EMPTY	2400
#define BATT_LEVEL_LOW		2500
#define BATT_LEVEL_ONE		2752
#define BATT_LEVEL_TWO		2832
#define BATT_LEVEL_THREE	2976

#define BATT_LEVEL_LOW_WHEN_RUN	    2220 //2460
#define BATT_LEVEL_EMPTY_WHEN_RUN	2000

//在某些情况下(比如报警界面)功耗特别大，所以阈值需要降低
#define BATT_LEVEL_EMPTY_AT_BIG_POWER 2400


#define WAKE_UP_SLAVER_PLUS_TIME_MS	50 


#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
	kBatteryNotExist,
	kBatteryEmpty,
    kBatteryLow,
    kBatteryOne,
    kBatteryTwo,
    kBatteryThree,
    kBatteryFour,

    kBatteryLevelMax
}BatteryLevel;

//所在功耗环境,用于灵活判断电池等级
typedef enum{
	kBatteryAtNormalPower,
	kBatteryAtBigPower,
}BatteryEnvironment;


/************************************************
* Declaration
************************************************/
bool sys_power_is_adapter_insert();
bool sys_power_is_battery_exist();

void sys_power_ADC_enable();

void sys_power_ADC_disable();

void sys_master_mcu_power_enable();

void sys_master_mcu_power_disable();

//获取系统电源的ADC值
int sys_power_get_sys_pwr_adc_value();

int sys_power_get_sys_pwr_adc_average_value();

//使能与失能系统电源，与key_pwr_on/off配合
void sys_power_enable();

void sys_power_disable();

//将电池的adc值转换成电压值并扩大1000倍
int sys_power_battery_voltage();

//输入电池电量返回电池电量等级
BatteryLevel sys_power_get_battery_level(BatteryEnvironment power_environment, int batt_vol);

//返回电池电压等级
BatteryLevel sys_power_battery_level(BatteryEnvironment power_environment);

void sys_power_wake_up_slaver();

void sys_power_wake_up_slaver_pin_high();

void sys_power_wake_up_slaver_pin_low();
#ifdef __cplusplus
}
#endif




