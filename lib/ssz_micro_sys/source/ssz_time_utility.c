#include "ssz_time_utility.h"
#include "ssz_common.h"
#include "ssz_str_utility.h"
#include <stdlib.h>

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/

/*days of each month*/
static const int8_t g_month_days[12] = { 
  31		/*JUN*/
, 28			/*FEB*/
, 31			/*MAR*/
, 30			/*APR*/
, 31			/*MAY*/
, 30			/*JUN*/
, 31			/*JUL*/
, 31			/*AUG*/
, 30			/*SEP*/
, 31			/*OCT*/
, 30			/*NOV*/
, 31			/*DEC*/
};

/************************************************
* Function 
************************************************/
/***********************************************
* Description:
*   is leap year
* Argument:
*   year:
*
* Return:
*
************************************************/
bool ssz_is_leap_year(int year)
{
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/****************************************************************
* Description:
*	get the max day in someone month.
*
*****************************************************************/
int ssz_max_days_in_month(int year, int month)
{
    int days;

	ssz_assert(month >= 1 && month <= 12);
    days = g_month_days[month - 1];
    /*check leap year and set the days of FEB*/
    if ((true == ssz_is_leap_year(year)) && (2 == month))
    {
        days = 29;
    }

    return days;
}

//calc the week day of the day
//return 0~6 -> sunday, monday,...
int ssz_week_day(int year, int month, int day)
{
	int y, c, m, d, week;
	int zeller;

	if (month >= 3)
	{
		m = month;
		y = year % 100;
		c = year / 100;
		d = day;
	}
	else	
	{
		m = month + 12;
		y = (year - 1) % 100;
		c = (year - 1) / 100;
		d = day;
	}

	zeller = y + y / 4 + c / 4 - 2 * c + (26 * (m + 1)) / 10 + d - 1;
	if (zeller < 0)    
	{
		week = 7 - (-zeller) % 7;
	}
	else
	{
		week = zeller % 7;
	}
	return week;
}

//the start time is SSZ_BASE_YEAR.1.1 00:00:00, like mktime
int32_t ssz_time_to_seconds(const SszDateTime *time){
	return ssz_time_to_seconds_ex(time, 0);
}

//the start time is SSZ_BASE_YEAR.1.1 00:00:00, like mktime
int32_t ssz_time_to_seconds_ex(const SszDateTime *time, int time_zone)
{
    int t;
    int32_t seccount = 0;

    ssz_assert(time->year >= SSZ_BASE_YEAR && time->month >= 1 && time->day >= 1);
    //add year
    for (t = SSZ_BASE_YEAR; t < time->year; t++)
    {
        if (ssz_is_leap_year(t))
        {
            seccount += 366 * 24 * 3600;//add leap year seconds
        }
        else
        {
            seccount += 365 * 24 * 3600;  //add normal year seconds
        }
    }

    //add month
    for (t = 0; t < time->month-1; t++)
    {
        seccount += g_month_days[t] * 24 * 3600; //add one month seconds
        if (ssz_is_leap_year(time->year) && t == 1)
        {
            seccount += 24 * 3600;//February at leap year add 1 day
        }
    }

    //add day,hout,minute,second
    seccount += (time->day - 1) * 24 * 3600;
    seccount += time->hour * 3600;
    seccount += time->minute * 60;
    seccount += time->second;
    seccount -= time_zone*3600;

    return seccount;
}

//the start time is SSZ_BASE_YEAR.1.1 00:00:00 , like gmtime or localtime
void ssz_seconds_to_time(int32_t seconds, SszDateTime *time){
	ssz_seconds_to_time_ex(seconds, time, 0);
}

//the start time is SSZ_BASE_YEAR.1.1 00:00:00 , like gmtime or localtime
void ssz_seconds_to_time_ex(int32_t seconds, SszDateTime *time, int time_zone)
{
	seconds += time_zone*3600;
    int32_t days = seconds / (24 * 3600);
    int32_t seconds_at_one_day = seconds % (24 * 3600);

    memset(time, 0, sizeof(SszDateTime));
    time->year = SSZ_BASE_YEAR;
    time->month = 1;
    time->day = 1;

    //calc the year
    while (days >= 365)
    {
        if (ssz_is_leap_year(time->year))
        {
            if (days >= 366)
            {
                days -= 366;
            }
            else
            {
                //in a year
                break;
            }
        }
        else
        {
            days -= 365;
        }

        time->year++;
    }

    //calc the month
    while (days >= 28)
    {
        if (ssz_is_leap_year(time->year) && time->month == 2)
        {
            if (days >= 29)
            {
                days -= 29;
            }
            else
            {
                //in a month
                break;
            }
        }
        else
        {
            if (days >= g_month_days[time->month - 1])
            {
                days -= g_month_days[time->month - 1];
            }
            else
            {
                //in a month
                break;
            }

        }

        time->month++;
    }

    time->day += (int8_t)days;
    time->hour = (int8_t)(seconds_at_one_day / 3600);
    time->minute = (int8_t)((seconds_at_one_day % 3600) / 60);
    time->second = (int8_t)((seconds_at_one_day % 3600) % 60);
}

//it will find three number as year, month, day
//e.g. "2015-01-23"  ->2015 1 23
SszDate ssz_str_to_date(const char* date_str, SszDateFormat date_format)
{
    SszDate date;
	int nums[3];
	
	memset(nums, 0, sizeof(nums));
	//convert str to integer
	ssz_str_to_numbers(date_str, nums, 3);
	switch (date_format) {
	case kSszDateFormat_YYYY_MM_DD:
		date.year = nums[0];
		date.month = nums[1];
		date.day = nums[2];
		break;
	case kSszDateFormat_DD_MM_YYYY:
		date.year = nums[2];
		date.month = nums[1];
		date.day = nums[0];
		break;
	case kSszDateFormat_MM_DD_YYYY:
		date.year = nums[2];
		date.month = nums[0];
		date.day = nums[1];
		break;
	default:
		ssz_assert_fail();
		break;
	}


    return date;
}

//it will find three number as hour, min, second
//e.g. "17:42:49"  ->17 42 49
SszTime ssz_str_to_time(const char* time_str)
{
    SszTime time1;
	int nums[3];

	memset(nums, 0, sizeof(nums));
    //convert str to integer
	ssz_str_to_numbers(time_str, nums, 3);
	time1.hour = nums[0];
	time1.minute = nums[1];
	time1.second = nums[2];

    return time1;
}

//convert to days, HH:MM:SS.xxx
//return rest milliseconds
int ssz_milliseconds_to_run_time_of_days(int32_t milliseconds, int *days, int *hour, int* minute, int *second) {
	int rest_ms = milliseconds % 1000;
	milliseconds = milliseconds / 1000;
	*days = milliseconds / (SSZ_SEC_IN_DAY);
	milliseconds %= SSZ_SEC_IN_DAY;
	*hour = milliseconds / (SSZ_SEC_IN_HOUR);
	milliseconds %= SSZ_SEC_IN_HOUR;
	*minute = milliseconds / (SSZ_SEC_IN_MIN);
	*second = milliseconds%SSZ_SEC_IN_MIN;
	return rest_ms;
}

//convert to hours, MM:SS.xxx
//return rest milliseconds
int ssz_milliseconds_to_run_time_of_hours(int32_t milliseconds, int *hours, int* minute, int *second) {
	int rest_ms = milliseconds % 1000;
	milliseconds = milliseconds / 1000;
	*hours = milliseconds / (SSZ_SEC_IN_HOUR);
	milliseconds %= SSZ_SEC_IN_HOUR;
	*minute = milliseconds / (SSZ_SEC_IN_MIN);
	*second = milliseconds%SSZ_SEC_IN_MIN;
	return rest_ms;
}