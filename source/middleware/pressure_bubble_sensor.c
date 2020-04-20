/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-20 dczhang
* Initial revision.
*
************************************************/
#include "pressure_bubble_sensor.h"
#include "ssz_gpio.h"
#include "alarm.h"



/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static bool g_pressure_bubble_sensor_is_pwr_enable = false;
/************************************************
* Function 
************************************************/
void pressure_and_bubble_sensor_pwr_enable()
{
    ssz_gpio_set(PRESSURE_BUBBLE_SENSOR_PWR_EN_GPIO_PORT,PRESSURE_BUBBLE_SENSOR_PWR_EN_PIN);
	g_pressure_bubble_sensor_is_pwr_enable = true;
}

void pressure_and_bubble_sensor_pwr_disable()
{
    ssz_gpio_clear(PRESSURE_BUBBLE_SENSOR_PWR_EN_GPIO_PORT,PRESSURE_BUBBLE_SENSOR_PWR_EN_PIN);
	g_pressure_bubble_sensor_is_pwr_enable = false;
}
bool pressure_and_bubble_sensor_is_pwr_enable() {
	return g_pressure_bubble_sensor_is_pwr_enable;
}

//如果产生气泡则返回E_TRUE
bool pressure_bubble_sensor_is_generate_bubble()
{
    if(ssz_gpio_is_high(BUBBLE_DETECT_GPIO_PORT,BUBBLE_DETECT_PIN))
        return false;
    else
        return true;
}

//获取压力传感器的电压值
int pressure_bubble_sensor_get_pressure_sensor_ADC_value()
{
    return mid_adc_get_ADC_channel_value(kPressureADC);
}

int pressure_bubble_sensor_get_pressure_sensor_ADC_average_value()
{
    return mid_adc_get_average_ADC_channel_value(kPressureADC);
}






