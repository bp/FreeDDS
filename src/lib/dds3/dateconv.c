static char rcsid[]="$Id: dateconv.c 177 2010-08-20 15:57:35Z user $";
/*
RCS-t=DDS, conversion between segy Date & usp SGRDat
*/

/******************************************/
/***   Copyright (C) 2003               ***/
/***   BP America, Houston, TX 77079    ***/
/***   Jerry Ehlers, Sep 2003           ***/
/******************************************/


#include "dds_sys.h"
#include <time.h>

static int n, m[5];
static time_t timeval;
static struct tm timestruct;
static int normal_months[] = { 31, 28, 31, 30, 31, 30, 
                               31, 31, 30, 31, 30, 31 };
static int leap_months[]   = { 31, 29, 31, 30, 31, 30,
                               31, 31, 30, 31, 30, 31 };


/*
 * Transform the date from int4 seconds back into SEGY form
 */

void ddsDate2Struct(void *datestruct, double *dateint) 
{
   /* convert to structure */
#if defined(_WIN32) || defined(MINGW)
   time(&timeval);
   timestruct = *(localtime(&timeval));
#else
   timeval = (time_t)*dateint;
   localtime_r(&timeval,&timestruct);
#endif
   /* host to SEGY structure */
   m[0] = timestruct.tm_year + 1900;
   m[1] = timestruct.tm_yday + 1;
   m[2] = timestruct.tm_hour;
   m[3] = timestruct.tm_min;
   m[4] = timestruct.tm_sec;
   CON_NAVV(DDS_STYPE_INTEGER2, DDS_CTYPE_INT, datestruct, m, 5);
}


/* 
 * Transform  the date from SEGY form back into int4 seconds
 */

void ddsDate2Int(void *dateint, void *datestruct)
{
   int yday, day, month, year;
   int *months, leap;

   /* SEGY to host structure */
   CON_ANVV(DDS_CTYPE_INT, DDS_STYPE_INTEGER2, m, datestruct, 5);
   timestruct.tm_year = m[0];
   timestruct.tm_yday = m[1];
   timestruct.tm_hour = m[2];
   timestruct.tm_min  = m[3];
   timestruct.tm_sec  = m[4];

   /* get correct months sizes */
   if (timestruct.tm_year >= 1900) timestruct.tm_year -= 1900;
   year = timestruct.tm_year + 1900;
   leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
   if (leap != 0) {
      months = leap_months;
   } else {
      months = normal_months;
   }

   /* fill in month & montheday structure */
   yday = timestruct.tm_yday;
   for (month=0;month<12,yday>0;month++) {
      day = yday;
      yday -= months[month];
      if (yday <= 0) {
         timestruct.tm_mon = month;
         timestruct.tm_mday = day;
      }
   }

   /* convert to seconds */
   timestruct.tm_isdst = -1;
   timeval = mktime(&timestruct);

   /* host to USP int */
   n = timeval;
   CON_NASS(DDS_STYPE_INTEGER4, DDS_CTYPE_INT, dateint, &n);
}
