static char rcsid[]="$Id: fake2float.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, convert fake float 4 ieee (two 16 bit chunks) to host float

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, 24 May 1997           ***/
/**********************************************/

#include "dds_sys.h"

void ddsFake2Float(
   float *fvalue,
   const int *hi,
   const int *lo)
{
   char tmphi[4], tmplo[4], buf[4];

   CON_AASS(DDS_STYPE_INTEGER4, DDS_CTYPE_INT, tmphi, hi);
   CON_AASS(DDS_STYPE_INTEGER4, DDS_CTYPE_INT, tmplo, lo);

   buf[0] = tmphi[2];
   buf[1] = tmphi[3];
   buf[2] = tmplo[2];
   buf[3] = tmplo[3];

   CON_AASS(DDS_CTYPE_FLOAT, DDS_STYPE_FLOAT4, fvalue, buf);

   return;
}
   


#ifdef OLD_VERSION
/* this is probably buggy for BIG_ENDIAN architectures */

#include "dds_sys.h"

void ddsFake2Float(
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

      buf_hi = (char*)&buf;
      buf_lo = buf_hi + 2;
   #endif

   #if (HOST_SHORT_DDS == DDS_STYPE_INTEGER2)
      memcpy((void*)buf_hi, (void*)hi, 2);
      memcpy((void*)buf_lo, (void*)lo, 2);
   #else
      memset((void*)buf_hi, '\0', sizeof(short));
      memset((void*)buf_lo, '\0', sizeof(short));
      memcpy((void*)buf_hi, (void*)((char*)hi + sizeof(short) - 2), 2);
      memcpy((void*)buf_lo, (void*)((char*)lo + sizeof(short) - 2), 2);
   #endif

   #if (HOST_FLOAT_DDS != DDS_STYPE_FLOAT)
      CON_AASS(DDS_CTYPE_FLOAT, DDS_STYPE_FLOAT4, fvalue, &buf);
   #endif

   return;
}
#endif /* OLD_VERSION */
