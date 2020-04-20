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




#define PRESSURE_BUBBLE_SENSOR_PWR_EN_PIN SENSOR_PWR_EN_Pin
#define PRESSURE_BUBBLE_SENSOR_PWR_EN_GPIO_PORT SENSOR_PWR_EN_GPIO_Port
#define BUBBLE_DETECT_PIN BUBBLE_DETECT_Pin
#define BUBBLE_DETECT_GPIO_PORT BUBBLE_DETECT_GPIO_Port




/************************************************
* Declaration
************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void pressure_and_bubble_sensor_pwr_enable();

void pressure_and_bubble_sensor_pwr_disable();

bool pressure_and_bubble_sensor_is_pwr_enable();

//如果产生气泡则返回E_TRUE
bool pressure_bubble_sensor_is_generate_bubble();

//获取压力传感器的电压值
int pressure_bubble_sensor_get_pressure_sensor_ADC_value();

//获取压力传感器的电压值+滤波
int pressure_bubble_sensor_get_pressure_sensor_ADC_average_value();




#ifdef __cplusplus
}
#endif


