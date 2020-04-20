/************************************************
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-06-15 jlli
* Initial revision.
*
************************************************/
#include "drv_rtc.h"
#include "ssz_sim_i2c.h"
#include "dev_def.h"
#include "ssz_utility.h"
#include "ssz_common.h"
#ifdef SSZ_TARGET_SIMULATOR
#include "sim_interface.h"
#include "ssz_time_utility.h"
#include "string.h"

#endif

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static SszSimI2C g_rtc_i2c;
/************************************************
* Function 
************************************************/


void drv_rtc_init()
{
	ssz_sim_i2c_master_init(&g_rtc_i2c, RTC_I2C_SCK_GPIO_Port, RTC_I2C_SCK_Pin,
		RTC_I2C_SDA_GPIO_Port, RTC_I2C_SDA_Pin,
		kSszGpioOutOD_PU, kSszGpioInputPU);
	//check if locked after reset
	if (ssz_sim_i2c_master_is_slaver_busy(&g_rtc_i2c)) {
		ssz_sim_i2c_master_force_slaver_release(&g_rtc_i2c);
	}
}

void drv_rtc_start()
{
    int sec;
    sec=drv_rtc_get_time_individually(kRtcSec);
    // enable osc start bit
    drv_rtc_set_time_individually(kRtcSec,sec+ssz_bcd_to_decimal(0x80));
}

void drv_rtc_stop()
{
    int sec;
    sec=drv_rtc_get_time_individually(kRtcSec);
    drv_rtc_set_time_individually(kRtcSec,sec);
}

int drv_rtc_set_time_individually(RtcTimeType type, int time_val)
{
    ssz_sim_i2c_start(&g_rtc_i2c);
    ssz_sim_i2c_write_byte(&g_rtc_i2c, RTC_WRITE);
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);
    
    switch (type){
        case kRtcSec:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCSEC);
            break;
        case kRtcMin:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCMIN);
            break;
        case kRtcHour:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCHOUR);           
            break;
        case kRtcWkday:
			if (time_val == 0) {
				time_val = 7;
			}
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCWKDAY);            
            break;
        case kRtcDate:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCDATE);            
            break;
        case kRtcMth:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCMTH);      
            break;
        case kRtcYear:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCYEAR);
            time_val=time_val- RTC_BASE_YEAR;
            break;
        default:
            return -1;
    }
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);
    ssz_sim_i2c_write_byte(&g_rtc_i2c, ssz_decimal_to_bcd(time_val));
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);   

    ssz_sim_i2c_stop(&g_rtc_i2c);
    
    return 0;
}

int drv_rtc_get_time_individually(RtcTimeType type)
{
    uint8_t tmp_read=0;

    ssz_sim_i2c_start(&g_rtc_i2c);
    ssz_sim_i2c_write_byte(&g_rtc_i2c, RTC_WRITE);
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    
    switch (type){
        case kRtcSec:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCSEC);
            break;
        case kRtcMin:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCMIN);
            break;
        case kRtcHour:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCHOUR);
            break;
        case kRtcWkday:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCWKDAY);
            break;
        case kRtcDate:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCDATE);
            break;
        case kRtcMth:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCMTH);
            break;
        case kRtcYear:
            ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCYEAR);
            break;
        default:
            return -1;
    }

    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    ssz_sim_i2c_start(&g_rtc_i2c);
    ssz_sim_i2c_write_byte(&g_rtc_i2c, RTC_READ);
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    tmp_read = ssz_sim_i2c_read_byte(&g_rtc_i2c);
    ssz_sim_i2c_send_nack(&g_rtc_i2c);
    ssz_sim_i2c_stop(&g_rtc_i2c);

    switch (type){
            case kRtcSec:
                tmp_read &= 0x7F;
                break;
            case kRtcMin:
                tmp_read &= 0x7F;
                break;
            case kRtcHour:
                tmp_read &= 0x3F;
                break;
            case kRtcWkday:
                tmp_read &= 0x07;
                break;
            case kRtcDate:
                tmp_read &= 0x3F;
                break;
            case kRtcMth:
                tmp_read &= 0x1F;
                break;
            case kRtcYear:
                break;
            default:
                break;
        }

    if(type == kRtcYear)
        return (ssz_bcd_to_decimal(tmp_read)+RTC_BASE_YEAR);
	else if (type == kRtcWkday) {
		int week_day = ssz_bcd_to_decimal(tmp_read);
		if (week_day==7) {
			week_day = 0;
		}
		return week_day;
	}
    else
        return ssz_bcd_to_decimal(tmp_read);
}

void drv_rtc_set_time_all(SszDateTime* time_info)
{
    drv_rtc_stop();

    ssz_sim_i2c_start(&g_rtc_i2c);
    ssz_sim_i2c_write_byte(&g_rtc_i2c, RTC_WRITE);
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCSEC);
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    ssz_sim_i2c_write_byte(&g_rtc_i2c, ssz_decimal_to_bcd(time_info->second));
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    ssz_sim_i2c_write_byte(&g_rtc_i2c, ssz_decimal_to_bcd(time_info->minute));
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    ssz_sim_i2c_write_byte(&g_rtc_i2c, ssz_decimal_to_bcd(time_info->hour));
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

	if (time_info->weekday == 0) {
		ssz_sim_i2c_write_byte(&g_rtc_i2c, 0x07);
	}
	else {
		ssz_sim_i2c_write_byte(&g_rtc_i2c, ssz_decimal_to_bcd(time_info->weekday));
	}
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    ssz_sim_i2c_write_byte(&g_rtc_i2c, ssz_decimal_to_bcd(time_info->day));
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    ssz_sim_i2c_write_byte(&g_rtc_i2c, ssz_decimal_to_bcd(time_info->month));
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);


    ssz_sim_i2c_write_byte(&g_rtc_i2c, ssz_decimal_to_bcd(time_info->year-RTC_BASE_YEAR));
    if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
        ssz_assert(0);

    ssz_sim_i2c_stop(&g_rtc_i2c);

    drv_rtc_start();

}

void drv_rtc_get_time_all(SszDateTime* rt_time)
{

  ssz_sim_i2c_start(&g_rtc_i2c);
  ssz_sim_i2c_write_byte(&g_rtc_i2c, RTC_WRITE);

  if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
      ssz_assert(0);

  ssz_sim_i2c_write_byte(&g_rtc_i2c, RTCSEC);
  if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
      ssz_assert(0);

  ssz_sim_i2c_start(&g_rtc_i2c);
  ssz_sim_i2c_write_byte(&g_rtc_i2c, RTC_READ);
  if(ssz_sim_i2c_read_ack(&g_rtc_i2c)!=0)
      ssz_assert(0);

  rt_time->second = ssz_bcd_to_decimal(ssz_sim_i2c_read_byte(&g_rtc_i2c) & 0x7F);
  ssz_sim_i2c_send_ack(&g_rtc_i2c);

  rt_time->minute = ssz_bcd_to_decimal(ssz_sim_i2c_read_byte(&g_rtc_i2c) & 0x7F);
  ssz_sim_i2c_send_ack(&g_rtc_i2c);

  rt_time->hour = ssz_bcd_to_decimal(ssz_sim_i2c_read_byte(&g_rtc_i2c) & 0x3F);
  ssz_sim_i2c_send_ack(&g_rtc_i2c);

  rt_time->weekday = ssz_bcd_to_decimal(ssz_sim_i2c_read_byte(&g_rtc_i2c)& 0x07);
  ssz_sim_i2c_send_ack(&g_rtc_i2c);

  rt_time->day = ssz_bcd_to_decimal(ssz_sim_i2c_read_byte(&g_rtc_i2c) & 0x3F);
  ssz_sim_i2c_send_ack(&g_rtc_i2c);

  rt_time->month = ssz_bcd_to_decimal(ssz_sim_i2c_read_byte(&g_rtc_i2c) & 0x1F);
  ssz_sim_i2c_send_ack(&g_rtc_i2c);

  rt_time->year = ssz_bcd_to_decimal(ssz_sim_i2c_read_byte(&g_rtc_i2c));
  ssz_sim_i2c_send_nack(&g_rtc_i2c);

  ssz_sim_i2c_stop(&g_rtc_i2c);
  
if (rt_time->weekday == 7) {
	  rt_time->weekday = 0;
  }
  rt_time->year += RTC_BASE_YEAR;
  
}
