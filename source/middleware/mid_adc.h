/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-15 dczhang
* Initial revision.
*
************************************************/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#include "ssz_def.h"
#include "dev_def.h"
#include "ssz_common.h"

#define ADC_GET_AVERAGE_NUMBER 10       //增大数值的同时要更改变量类型，防止溢出


#define ADC_VREF_EN_PIN VREF_EN_Pin
#define ADC_VREF_EN_PORT VREF_EN_GPIO_Port



typedef enum{
    kSysPwrADC,             //系统电流检测
    kPressureADC,           //压力传感器
    kSZDJCurrentADC,        //输注电机电流检测
    kSTFDJCurrentADC,       //三通阀电机电流检测
    kSlaverMCUPwrADC,       //从电源电压检测
}ADCChannelChoose;


/************************************************
* Declaration
************************************************/
void mid_adc_init();
void mid_adc_vref_enable();
void mia_adc_vref_disable();




int mid_adc_get_ADC_channel_value(ADCChannelChoose adc_channel);

int mid_adc_get_average_ADC_channel_value(ADCChannelChoose adc_channel);


#ifdef __cplusplus
}
#endif



