static char rcsid[]="$Id: float2fake.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, convert host float to fake float 4 ieee (two 16 bit chunks)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, 24 May 1997           ***/
/**********************************************/

#include "dds_sys.h"

void ddsFloat2Fake(
   const float *fvalue,
   int *hi,
   int *lo)
{
   char tmphi[4], tmplo[4], buf[4];

   CON_AASS(DDS_STYPE_FLOAT4, DDS_CTYPE_FLOAT, buf, fvalue);

   tmphi[0] = '\0';
   tmphi[1] = '\0';
   tmphi[2] = buf[0];
   tmphi[3] = buf[1];
   tmplo[0] = '\0';
   tmplo[1] = '\0';
   tmplo[2] = buf[2];
   tmplo[3] = buf[3];

   CON_AASS(DDS_CTYPE_INT, DDS_STYPE_INTEGER4, hi, tmphi);
   CON_AASS(DDS_CTYPE_INT, DDS_STYPE_INTEGER4, lo, tmplo);

   return;
}

#ifdef OLD_VERSION
/* this is probably buggy for BIG_ENDIAN architectures */

#include "dds_sys.h"

void ddsFloat2Fake(
   float *fvalue,
   short *hi,
   short *lo)
{
   char *buf_hi, *buf_lo;

   #if (HOST_FLOAT_DDS == DDS_STYPE_FLOAT)
      buf_hi = (char*)fvalue;
      buf_lo = buf_hi + 2;
   #else
      long buf;

      CON_AASS(DDS_STYPE_FLOAT4, DDS_CTYPE_FLOAT, &buf, fvalue);

      buf_hi = (char*)&buf;
      buf_lo = buf_hi + 2;
   #endif

   #if (HOST_SHORT_DDS == DDS_STYPE_INTEGER2)
      memcpy((void*)hi, (void*)buf_hi, 2);
      memcpy((void*)lo, (void*)buf_lo, 2);
   #else
      memset((void*)buf_hi, '\0', sizeof(short));
      memset((void*)buf_lo, '\0', sizeof(short));
      memcpy((void*)((char*)hi + sizeof(short) - 2), (void*)buf_hi, 2);
      memcpy((void*)((char*)lo + sizeof(short) - 2), (void*)buf_lo, 2);
   #endif

   return;
}
#endif /* OLD_VERSION */
