#pragma once

//check if user define below macro at compile option
#ifdef SSZ_TARGET_SIMULATOR
//simulator
#define SSZ_TARGET_SIMULATOR 1 //it is used to let IDE know this and tip it when write code
#elif defined SSZ_TARGET_MACHINE
//not simulator
#define SSZ_TARGET_MACHINE 1 //it is used to let IDE know this and tip it when write code
#else
#error "need define SSZ_TARGET_SIMULATOR or SSZ_TARGET_MACHINE"
#endif

//define the machine target platform
#ifdef SSZ_TARGET_MACHINE
//valid values:
//#define SSZ_TARGET_STM32F4XX_HAL
//#define SSZ_TARGET_STM32F1XX_HAL
#define SSZ_TARGET_STM32L1XX_HAL
//#define SSZ_TARGET_STM32L0XX_HAL
#endif

//we rely on the RTC chip, because the weekday and leap year is calculated by RTC
//the base year should same as RTC chip's base year,
//if not same, when read from RTC, need check if the year is eraly than this base year,
//if early, need change the RTC time as not early than this base year
#define SSZ_BASE_YEAR 2000  //it support 68 years from base year

//define the temp buff size, refer ssz_tmp_buff_alloc
#define SSZ_COMMON_TMP_BUFF_MAX_SIZE 512

//define the backup tmp buff when write flash
// general the size is same as nvram one erase unit buff size, 
//used for flash which need erase before write
// if not use flash or no need backup, can set it as 0
#define SSZ_BACKUP_TMP_BUFF_WHEN_WRITE_FLASH 4096

//definition of ssz_uart_read,ssz_uart_write at simulator
//1:use ssz's definition, 0: not use
#define SSZ_USE_DEFAULT_UART_DEFINITION_AT_SIMULATOR 0

//1:enable precision tick time 0:disable
#define SSZ_PRECISION_TICK_TIME_MODULE_ENABLE 1

//the time str format when print or trace
//0: ms value, based on tick time, e.g 3500
//1: seconds.xxx, based on tick time, e.g 3.500
//2: hours:mm:ss.xxx, based on tick time, e.g.  0:00:03.500
//3: days, hh:mm:ss.xxx, based on tick time, e.g.  0 days, 00:00:03.500
//4: RTC time, hh:mm:ss, based on RTC, e.g.  12:00:03
//5: RTC date time, YYYY-MM-DD hh:mm:ss, based on RTC, e.g. 2017-12-28 12:00:03
#ifdef SSZ_TARGET_MACHINE
#define SSZ_TIME_STR_FORMAT_WHEN_PRINT 0
#else
#define SSZ_TIME_STR_FORMAT_WHEN_PRINT 2
#endif



