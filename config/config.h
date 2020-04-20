#pragma once

#include "ssz_config.h"
#include "dev_def.h"


#ifdef __cplusplus
extern "C" {
#endif


//define how many milliseconds per tick
#define MSECS_PER_TICK 1
//define how many ticks per second
#define TICKS_PER_SEC  (1000/MSECS_PER_TICK)

//is enable cpu usage calc, if enable, SSZ_PRECISION_TICK_TIME_MODULE_ENABLE need enable too
#define CPU_USAGE_MODULE_ENABLE 1

// if enable, SSZ_PRECISION_TICK_TIME_MODULE_ENABLE need enable too
#define STOP_WATCH_MODULE_ENABLE 1

//if enable dynamic timer
#define DTIMER_MODULE_ENABLE 1
#if DTIMER_MODULE_ENABLE
//define dynamic timer max count	
#define DTIMER_MAX_COUNT 2	
#endif

#define RTC_MODULE_ENABLE 1
#define DATA_MODULE_ENABLE 1
#define KEY_MODULE_ENABLE 1
#define SCREEN_MODULE_ENABLE 0
#define RECORD_LOG_MODULE_ENABLE 1
#define PC_COM_MODULE_ENABLE 1
#define LOW_POWER_TEST_ENABLE 0

//is enable ui
#define UI_MODULE_ENABLE 1

//is enable ui view
#define UI_VIEW_MODULE_ENABLE 0

//the max year support
#define MAX_YEAR (SSZ_BASE_YEAR+68)

//define flash erase unit size
#define FLASH_ERASE_UNIT_SIZE 4096
#if SSZ_BACKUP_TMP_BUFF_WHEN_WRITE_FLASH>0 && SSZ_BACKUP_TMP_BUFF_WHEN_WRITE_FLASH!=FLASH_ERASE_UNIT_SIZE
#error "the nvram backup size is not same as flash erase unit size"
#endif

//sst25 flash define
#define SST25_VF020
//#define SST25_VF010

#define SST25_FLASH_CS_PORT      FLASH_NSS_GPIO_Port
#define SST25_FLASH_CS_PIN       FLASH_NSS_Pin



#ifdef __cplusplus
}
#endif



