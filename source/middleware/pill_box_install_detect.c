/************************************************
* DESCRIPTION:
*   药盒装夹传感器电源上电和掉电，
*   以及左右两端到位检测
* REVISION HISTORY:
*   Rev 1.0 2017-06-20 dczhang
* Initial revision.
*
************************************************/
#include "pill_box_install_detect.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
void pill_box_install_sensor_init()
{
    ssz_gpio_clear(PILL_BOX_INSTALL_PWR_EN_LEFT_PORT,PILL_BOX_INSTALL_PWR_EN_LEFT_PIN);
    
    ssz_gpio_clear(PILL_BOX_INSTALL_PWR_EN_RIGHT_PORT,PILL_BOX_INSTALL_PWR_EN_RIGHT_PIN);
}

//药盒装夹到位检测传感器电源使能
void pill_box_install_sensor_left_pwr_enable()
{
    ssz_gpio_set(PILL_BOX_INSTALL_PWR_EN_LEFT_PORT,PILL_BOX_INSTALL_PWR_EN_LEFT_PIN);
}

void pill_box_install_sensor_right_pwr_enable()
{
    ssz_gpio_set(PILL_BOX_INSTALL_PWR_EN_RIGHT_PORT,PILL_BOX_INSTALL_PWR_EN_RIGHT_PIN);
}


void pill_box_install_sensor_left_pwr_disable()
{
    ssz_gpio_clear(PILL_BOX_INSTALL_PWR_EN_LEFT_PORT,PILL_BOX_INSTALL_PWR_EN_LEFT_PIN);
}

void pill_box_install_sensor_right_pwr_disable()
{
    ssz_gpio_clear(PILL_BOX_INSTALL_PWR_EN_RIGHT_PORT,PILL_BOX_INSTALL_PWR_EN_RIGHT_PIN);
}


//到位则返回 E_TRUE
bool pill_box_install_left_detect()
{
    if(ssz_gpio_is_high(PILL_BOX_INSTALL_SENSOR_PORT,PILL_BOX_INSTALL_SENSOR_LEFT_PIN))
        return false ;
    else
        return true ;
}

//到位则返回 E_TRUE
bool pill_box_install_right_detect()
{
    if(ssz_gpio_is_high(PILL_BOX_INSTALL_SENSOR_PORT,PILL_BOX_INSTALL_SENSOR_RIGHT_PIN))
        return false;
    else
        return true;
}

