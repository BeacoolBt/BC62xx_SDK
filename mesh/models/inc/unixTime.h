#ifndef __UNIXTIME_H__
#define __UNIXTIME_H__

#include <stdio.h>
#include <stdbool.h>

#define UTC_BASE_YEAR 1970
#define MONTH_PER_YEAR 12
#define DAY_PER_YEAR 365
#define SEC_PER_DAY 86400
#define SEC_PER_HOUR 3600
#define SEC_PER_MIN 60

typedef struct
{
    unsigned short nYear;
    unsigned char nMonth;
    unsigned char nDay;
    unsigned char nHour;
    unsigned char nMin;
    unsigned char nSec;
    unsigned char DayIndex; /* 0 = Sunday */
} time_struct;


unsigned char applib_dt_is_leap_year(unsigned short year);

unsigned char applib_dt_last_day_of_mon(unsigned char month, unsigned short year);

unsigned char applib_dt_dayindex(unsigned short year, unsigned char month, unsigned char day);

void utc_sec_2_mytime(unsigned int utc_sec, time_struct *result, bool daylightSaving,uint8_t zone);

unsigned int mytime_2_utc_sec(time_struct *currTime, bool daylightSaving);

#endif/*__UNIXTIME_H__*/
