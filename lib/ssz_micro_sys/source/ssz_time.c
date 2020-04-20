#include "ssz_time.h"
#include "ssz_time_utility.h"
#include "ssz_config.h"
#include "time.h"
#include "ssz_common.h"
#include <stdlib.h>
#include "ssz_tick_time.h"
/************************************************
* Declaration
************************************************/

static char g_time_now_str[20];
static char g_time_now_h_str[9];
static SszDateTime g_time_last_read_str_time;
static SszDateTime g_time_last_read_str_time2;
/************************************************
* Variable 
************************************************/

/************************************************
* Function 
************************************************/
const char* ssz_time_now_str() {
	if (memcmp(ssz_time_now(), &g_time_last_read_str_time, sizeof(g_time_last_read_str_time)) != 0) {
		g_time_last_read_str_time = *ssz_time_now();
		snprintf(g_time_now_str, sizeof(g_time_now_str), "%d-%02d-%02d %02d:%02d:%02d",
			g_time_last_read_str_time.year, g_time_last_read_str_time.month, g_time_last_read_str_time.day,
			g_time_last_read_str_time.hour, g_time_last_read_str_time.minute, g_time_last_read_str_time.second);
	}

	return g_time_now_str;
}

//return now time str, no date, e.g. 22:45:30
const char* ssz_time_now_h_str() {
	if (memcmp(ssz_time_now(), &g_time_last_read_str_time2, sizeof(g_time_last_read_str_time2)) != 0) {
		g_time_last_read_str_time2 = *ssz_time_now();
		snprintf(g_time_now_h_str, sizeof(g_time_now_h_str), "%02d:%02d:%02d",
			g_time_last_read_str_time2.hour, g_time_last_read_str_time2.minute, g_time_last_read_str_time2.second);
	}

	return g_time_now_h_str;
}

//return the str of time, the time is null, return now time str
const char* ssz_time_str(const SszDateTime *in_time, char buff[SSZ_TIME_STR_SIZE]) {
	const SszDateTime* p;
	if (in_time == NULL) {
		p = ssz_time_now();
	}
	else {
		p = in_time;
	}
	snprintf(buff, 20, "%d-%02d-%02d %02d:%02d:%02d",
		p->year, p->month, p->day, p->hour, p->minute, p->second);

	return buff;
}

//return the str of time, the time is null, return now time str
const char* ssz_time_h_str(const SszTime *in_time, char buff[SSZ_TIME_H_STR_SIZE]){
	if (in_time == NULL) {
		const SszDateTime *tim;

		tim = ssz_time_now();
		snprintf(buff, 10, "%02d:%02d:%02d",
			tim->hour, tim->minute, tim->second);
	}
	else {
		snprintf(buff, 10, "%02d:%02d:%02d",
			in_time->hour, in_time->minute, in_time->second);
	}


	return buff;
}



//like strftime
// Format:
/*Y	writes year as a decimal number, e.g. 2017
*y	writes last 2 digits of year as a decimal number (range [00,99])
*
*b	writes abbreviated month name, e.g. Oct (locale dependent)
*B	writes full month name, e.g. October (locale dependent)
*m	writes month as a decimal number (range [01,12])
*
*d	writes day of the month as a decimal number (range [01,31])
*
*a	writes abbreviated weekday name, e.g. Fri (locale dependent)
*A	writes full weekday name, e.g. Friday (locale dependent)
*w	writes weekday as a decimal number, where Sunday is 0 (range [0-6])
*
*H	writes hour as a decimal number, 24 hour clock (range [00-23])
*I	writes hour as a decimal number, 12 hour clock (range [01,12])
*M	writes minute as a decimal number (range [00,59])
*S	writes second as a decimal number (range [00,60])
*
*c	writes standard date and time string, e.g. Sun Oct 17 04:41:13 2010 (locale dependent)
*D	equivalent to "%m/%d/%y"
*F	equivalent to "%Y-%m-%d" (the ISO 8601 date format)
*R	equivalent to "%H:%M"
*T	equivalent to "%H:%M:%S" (the ISO 8601 time format)
*p	writes localized a.m. or p.m. (locale dependent)
*
* refer http://en.cppreference.com/w/c/chrono/strftime
*return the bytes to write
*/
int ssz_time_format(char* str, int str_size, const char* format, const SszDateTime *in_time) {
	struct tm time2;

	memset(&time2, 0, sizeof(time2));
	time2.tm_year = in_time->year-1900;
	time2.tm_mon = in_time->month - 1;
	time2.tm_mday = in_time->day;
	time2.tm_hour = in_time->hour;
	time2.tm_min = in_time->minute;
	time2.tm_sec = in_time->second;
	time2.tm_wday = in_time->weekday;

	return strftime(str, str_size, format, &time2);
}
