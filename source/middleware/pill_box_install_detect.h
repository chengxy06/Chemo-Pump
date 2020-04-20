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

#ifdef __cplusplus
extern "C" {
#endif


#include "ssz_def.h"
#include "ssz_gpio.h"
#include "dev_def.h"


#define PILL_BOX_INSTALL_PWR_EN_LEFT_PORT YH_ZJ_PWR_EN_LEFT_GPIO_Port
#define PILL_BOX_INSTALL_PWR_EN_LEFT_PIN YH_ZJ_PWR_EN_LEFT_Pin
#define PILL_BOX_INSTALL_PWR_EN_RIGHT_PORT YH_ZJ_PWR_EN_RIGHT_GPIO_Port
#define PILL_BOX_INSTALL_PWR_EN_RIGHT_PIN YH_ZJ_PWR_EN_RIGHT_Pin

#define PILL_BOX_INSTALL_SENSOR_PORT YH_ZJ_LEFT_GPIO_Port
#define PILL_BOX_INSTALL_SENSOR_LEFT_PIN YH_ZJ_LEFT_Pin
#define PILL_BOX_INSTALL_SENSOR_RIGHT_PIN YH_ZJ_RIGHT_Pin



/************************************************
* Declaration
************************************************/
void pill_box_install_sensor_init();

//药盒装夹到位检测传感器电源使能
void pill_box_install_sensor_left_pwr_enable();

void pill_box_install_sensor_right_pwr_enable();


void pill_box_install_sensor_left_pwr_disable();

void pill_box_install_sensor_right_pwr_disable();


//到位则返回 E_TRUE
bool pill_box_install_left_detect();

//到位则返回 E_TRUE
bool pill_box_install_right_detect();


#ifdef __cplusplus
}
#endif



