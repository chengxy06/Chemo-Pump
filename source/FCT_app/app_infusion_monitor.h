/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-08-27 xqzhao
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "msg.h"

/************************************************
* Declaration
************************************************/

#define infusion_monitor_printfln(...) ssz_printfln_ex(is_module_enable_output(kModuleInfusionMonitor),INFUSION_MONITOR_MODULE_NAME,__VA_ARGS__)


//可以组合使用,哪个设置了就监视哪个
#define INFUSION_MONITOR_OCCLUSION_FLAG 0x01
#define INFUSION_MONITOR_BUBBLE_FLAG 0x02
#define INFUSION_MONITOR_NEAR_EMPTY_FLAG 0x04
#define INFUSION_MONITOR_INSTALL_NOT_IN_PLACE_FLAG 0x08
#define INFUSION_MONITOR_ALL_FLAG 0xFF

//infusion_flag: e.g. INFUSION_MONITOR_OCCLUSION_FLAG|INFUSION_MONITOR_BUBBLE_FLAG
void app_infusion_monitor_start(uint8_t monitor_flag);
void app_infusion_monitor_stop();
//用于指示监视哪些东西
uint8_t app_infusion_monitor_flag();

void app_infusion_monitor_init();

