/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-20 dczhang
* Initial revision.
*
************************************************/
#include "sys_power.h"
#include "common.h"
#include "timer.h"
#include "param.h"


/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
//适配器插入，返回E_TRUE
bool sys_power_is_adapter_insert()
{
    if(ssz_gpio_is_high(SYS_PWR_COM_PORT,SYS_PWR_ADAPTER_INSERT_PIN))
        return true;
    else
        return false;
}
bool sys_power_is_battery_exist() {
	return true;
}

void sys_power_wake_up_slaver_pin_high()
{
    ssz_gpio_set(S_WKUP_GPIO_Port,S_WKUP_Pin);
}

void sys_power_wake_up_slaver_pin_low()
{
    ssz_gpio_clear(S_WKUP_GPIO_Port,S_WKUP_Pin);
}

void sys_power_wake_up_slaver()
{
	sys_power_wake_up_slaver_pin_low();
	ssz_delay_ms(3);
	sys_power_wake_up_slaver_pin_high();
	//timer_set_handler(kTimerWakeUpSlaverMcutime, sys_power_wake_up_slaver_pin_high);
	//timer_start_oneshot_after(kTimerWakeUpSlaverMcutime, WAKE_UP_SLAVER_PLUS_TIME_MS);	
}

void sys_power_ADC_enable()
{
    ssz_gpio_set(SYS_PWR_COM_PORT,SYS_PWR_ADC_EN_PIN);
}

void sys_power_ADC_disable()
{
    ssz_gpio_clear(SYS_PWR_COM_PORT,SYS_PWR_ADC_EN_PIN);
}

//获取系统电源的ADC值
int sys_power_get_sys_pwr_adc_value()
{
    return mid_adc_get_ADC_channel_value(kSysPwrADC);
}

int sys_power_get_sys_pwr_adc_average_value()
{
    return mid_adc_get_average_ADC_channel_value(kSysPwrADC);
}

//使能与失能系统电源，与key_pwr_on/off配合
void sys_power_enable()
{
    ssz_gpio_set(SYS_PWR_COM_PORT,SYS_PWR_EN_PIN);
}

void sys_power_disable()
{
    ssz_gpio_clear(SYS_PWR_COM_PORT,SYS_PWR_EN_PIN);
#ifdef SSZ_TARGET_SIMULATOR
	sim_power_off();
#endif
}

// 
void sys_master_mcu_power_enable()
{
    ssz_gpio_set(M_PWR_EN_GPIO_Port,M_PWR_EN_Pin);
}
void sys_master_mcu_power_disable()
{
    ssz_gpio_clear(M_PWR_EN_GPIO_Port,M_PWR_EN_Pin);
}


//将电池的adc值转换成电压值并扩大1000倍
int sys_power_battery_voltage()
{
    int bat_vol;

    bat_vol = sys_power_get_sys_pwr_adc_average_value();

    bat_vol = M_get_battery_voltage_from_adc(bat_vol - g_battery_offset_ad);

    return bat_vol;
}

//输入电池电量返回电池电量等级
BatteryLevel sys_power_get_battery_level(BatteryEnvironment power_environment, int batt_vol)
{
    BatteryLevel batt_lvl = kBatteryFour;
    
	if (batt_vol <= BATT_LEVEL_NOT_EXIST) {
		batt_lvl = kBatteryNotExist;
    } else if ( (power_environment==kBatteryAtNormalPower&&batt_vol <= BATT_LEVEL_EMPTY) ||
			(power_environment==kBatteryAtBigPower&&batt_vol <= BATT_LEVEL_EMPTY_AT_BIG_POWER) 
		) {
		batt_lvl = kBatteryEmpty;
    } else if (batt_vol <= BATT_LEVEL_LOW) {
		batt_lvl = kBatteryLow;
    } else if (batt_vol <= BATT_LEVEL_ONE) {
		batt_lvl = kBatteryOne;
    } else if (batt_vol <= BATT_LEVEL_TWO) {
		batt_lvl = kBatteryTwo;
    } else if (batt_vol <= BATT_LEVEL_THREE) {
		batt_lvl = kBatteryThree;
    } else {
		batt_lvl = kBatteryFour;
    }
    
	return batt_lvl;
}

//返回电池电压等级
BatteryLevel sys_power_battery_level(BatteryEnvironment power_environment)
{
    int batt_vol;
    BatteryLevel batt_lvl;
   
    batt_vol = sys_power_battery_voltage();
    //printf("vol: %d\n", batt_vol);

    batt_lvl = sys_power_get_battery_level(power_environment, batt_vol);

    return batt_lvl;
}


