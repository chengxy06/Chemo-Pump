/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-15 jlli
* Initial revision.
*
************************************************/
#pragma once

#include "ssz_def.h"
#include "ssz_time.h"
/************************************************
* Declaration
************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#define RTC_BASE_YEAR 2000

//RTC register address
#define RTCSEC      0x00
#define RTCMIN      0x01
#define RTCHOUR     0x02
#define RTCWKDAY    0x03
#define RTCDATE     0x04
#define RTCMTH      0x05
#define RTCYEAR     0x06
#define CONTROL     0x07
#define OSCTRIM     0x08

// CONTROL register definition
// BIT7 | BIT6  |  BIT5  |  BIT4  |  BIT3  |   BIT2  |  BIT1  |  BIT0  |
// ---------------------------------------------------------------------
// OUT  | SQWEN | ALM1EN | ALM0EN | EXTOSC | CRSTRIM | SQWFS1 | SQWFS0 |
// ---------------------------------------------------------------------

#define RTC_ST_BIT          0x80   // RTCSEC register
#define RTC_12HR_BIT        0x40   // RTCHOUR register
#define RTC_PM_BIT          0x20   // RTCHOUR register
#define RTC_OSCRUN_BIT      0x20   // RTCWKDAY register
#define RTC_LPYR_BIT        0x20   // RTCMTH register


//control byte
#define RTC_READ    0xDF
#define RTC_WRITE   0xDE

typedef enum
{
    kRtcSec,
    kRtcMin,
    kRtcHour,
    kRtcWkday,
    kRtcDate,
    kRtcMth,
    kRtcYear,
    kRtcMax,
}RtcTimeType;

//typedef struct  
//{
//    int sec;      //0-59s
//	int min;      //0-59min
//    int hour;     //0-24h
//    int wkday;    //Sunday=7, Monday=1
//    int date;     //0-28,29,30,31
//    int month;    //0-12
//    int year;     //2000-2399
//}RtcTimeInfo;

void drv_rtc_init();

//start rtc
void drv_rtc_start();
//stop rtc
void drv_rtc_stop();

//set and get time infomation individually
int drv_rtc_set_time_individually(RtcTimeType type, int time_val);
int drv_rtc_get_time_individually(RtcTimeType type);

//set and get all time infomation 
void drv_rtc_set_time_all(SszDateTime* time_info);
void drv_rtc_get_time_all(SszDateTime* rt_time);

#ifdef __cplusplus
}
#endif



