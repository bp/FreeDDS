static char rcsid[]="$Id: cx_wtime.c 3 2008-12-08 20:35:59Z user $";
/*
RCS-t=DDS API, wall-time.

*/

/***************************************/
/***   Copyright (C) 2008            ***/
/***   BP Corporation, Houston, TX   ***/
/***   Jerry Ehlers, Oct 2008        ***/
/***************************************/

#include "dds_sys.h"
#include "dds_time.h"

/* 2 Mar 2007 RLS
 * Phuong Vu's "second.c" hacked from single to double precision.
 *
 * Note: 86400 = 24 hr * 60 min/hr * 60 sec/min = about 6 digits.
 * Single precision floats are only good to about 7 digits.
 * The old single precision version could only resolve approximately
 * 0.1 sec after one day of elapsed time... not good enough.
 */

double cddx_wtime()
{
   long n_usec;
   double time;
   time_t n_sec;
   struct timeval s_val;
#if defined(_WIN32) || defined(MINGW)
   char s_z;
#else
   struct timezone s_z;
#endif

   static time_t zero_sec = 0;
   static long zero_usec = 0;

   DDS_API_ENTRY("xwtime")

   gettimeofday(&s_val, &s_z);
   n_sec = s_val.tv_sec;
   n_usec = s_val.tv_usec;
   if (zero_sec == 0) {
      zero_sec = n_sec;
      zero_usec = n_usec;
   }
   time = (float) (n_sec - zero_sec) + 
          (float) (n_usec - zero_usec) * 1e-6;

   DDS_API_EXIT(time)
}

