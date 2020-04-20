/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-15 dczhang
* Initial revision.
*
************************************************/
#include "mid_adc.h"
#include "data.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
#ifdef SSZ_TARGET_SIMULATOR
int g_adc_channel;
#endif

/************************************************
* Function 
************************************************/
void mid_adc_vref_enable()
{
    ssz_gpio_set(ADC_VREF_EN_PORT,ADC_VREF_EN_PIN);
}

void mia_adc_vref_disable()
{
    ssz_gpio_clear(ADC_VREF_EN_PORT,ADC_VREF_EN_PIN);
}
void mid_adc_init()
{
    mid_adc_vref_enable();
#ifdef SSZ_TARGET_MACHINE
    //MX_ADC_Init();
#endif
}



int mid_adc_start_and_get_ADC_value()
{

    int ADC_Converted_Value = 0;

#ifdef SSZ_TARGET_MACHINE
    //启动转换
    if (HAL_ADC_Start(&ADC_COM) != HAL_OK)
    {
    /* Start Conversation Error */
        Error_Handler();
    }

    //等待转换结束，防止其他的转换正在进行
    if (HAL_ADC_PollForConversion(&ADC_COM, 10) != HAL_OK)
    {
    /* End Of Conversion flag not set on time */
        Error_Handler();
    }

    //检查是否结束
    if ((HAL_ADC_GetState(&ADC_COM) & HAL_ADC_STATE_EOC_REG) == HAL_ADC_STATE_EOC_REG)
    {
    //获取转换结果
        ADC_Converted_Value = HAL_ADC_GetValue(&ADC_COM);
    }
#else
	ADC_Converted_Value = sim_adc_get_value(1, g_adc_channel);
#endif
    return ADC_Converted_Value;
}


void mid_adc_config_ADC_channel(ADCChannelChoose adc_channel)
{
#ifdef SSZ_TARGET_MACHINE
    ADC_ChannelConfTypeDef sConfig;
#endif
    //选择通道
    switch(adc_channel)
    {
        case kSysPwrADC:
#ifdef SSZ_TARGET_MACHINE
            sConfig.Channel = ADC_CHANNEL_0;
#else
			g_adc_channel = 0;
#endif
            break;
        case kPressureADC:
#ifdef SSZ_TARGET_MACHINE
            sConfig.Channel = ADC_CHANNEL_9;
#else
            g_adc_channel = 9;
#endif
            break;
        case kSZDJCurrentADC:
#ifdef SSZ_TARGET_MACHINE
            sConfig.Channel = ADC_CHANNEL_8;
#else
            g_adc_channel = 8;
#endif
            break;
		
        case kSlaverMCUPwrADC:
#ifdef SSZ_TARGET_MACHINE
            sConfig.Channel = ADC_CHANNEL_20;
#else
            g_adc_channel = 20;
#endif
            break;			

        case kSTFDJCurrentADC:
#ifdef SSZ_TARGET_MACHINE
            sConfig.Channel = ADC_CHANNEL_22;
#else
            g_adc_channel = 22;
#endif
            break;
        default:
#ifdef SSZ_TARGET_MACHINE
            sConfig.Channel = ADC_CHANNEL_0;
			ssz_assert_fail();
#endif
            break;
    }
#ifdef SSZ_TARGET_MACHINE
    sConfig.Rank         = 1;                            //通道等级
    sConfig.SamplingTime = ADC_SAMPLETIME_4CYCLES;      //采样时间 

    //等待配置完成
    if (HAL_ADC_ConfigChannel(&ADC_COM, &sConfig) != HAL_OK)
    {
    /* Channel Configuration Error */
    Error_Handler();
    }
#endif    
}

int mid_adc_get_ADC_channel_value(ADCChannelChoose adc_channel)
{
    int adc_value = 0;
    mid_adc_config_ADC_channel(adc_channel);
    adc_value = mid_adc_start_and_get_ADC_value();

    return adc_value;
}

int mid_adc_get_average_ADC_channel_value(ADCChannelChoose adc_channel)
{
    int i;
    int adc_avg_value,adc_sum_value,adc_value_max,adc_value_min;
    int adc_value_num[ADC_GET_AVERAGE_NUMBER] = {0};
    int adc_value_offset = 0;   //偏移量

    mid_adc_config_ADC_channel(adc_channel);
    
    adc_value_num[0] = mid_adc_start_and_get_ADC_value();
    adc_value_max = adc_value_num[0];
    adc_value_min = adc_value_num[0];
    adc_sum_value = adc_value_num[0];

    for(i = 1;i < ADC_GET_AVERAGE_NUMBER;i++){
        adc_value_num[i] = mid_adc_start_and_get_ADC_value();
        
        if(adc_value_num[i] < adc_value_min)
            adc_value_min = adc_value_num[i];
        if(adc_value_num[i] > adc_value_max)
            adc_value_max = adc_value_num[i];

        adc_sum_value += adc_value_num[i];
    }

    adc_sum_value -= (adc_value_max + adc_value_min);
    adc_avg_value = adc_sum_value / (ADC_GET_AVERAGE_NUMBER - 2);

    //add the adc offset
    switch(adc_channel){
        case kSysPwrADC:
            //adc_value_offset = data_read_int(kDataBatteryRedress);
            break;
        //add motor adc offset

        default:
            adc_value_offset = 0;
            break;
    }

    adc_avg_value += adc_value_offset;

    return adc_avg_value;
}


