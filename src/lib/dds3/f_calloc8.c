static char rcsid[]="$Id: f_calloc8.c 148 2010-04-22 17:22:32Z user $";
/*
RCS-t=DDS API, clear malloc memory with error detection.

CAUTION: this function returns a pointer, not an integer.
On the SGI in 64 bit mode, pointer are 64 bit, while
integers are 32 bit.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_PTR
HOST_FTN_FUNC(fdds_calloc8, FDDS_CALLOC8)(
   HOST_FTN_INT *nobj,
   HOST_FTN_DBL *size8)
{
   HOST_FTN_DBL tmp8;
   size_t size;
   
   DDS_API_INIT;

   size = *size8;
   tmp8 = size;
   if (tmp8 != *size8) {
      ddsPanic("(fdds_calloc8) system cannot handle this size (%lg != %lg)\n",
         *size8, tmp8);
   }

   return (HOST_FTN_PTR)cdds_calloc((size_t)*nobj, size);
}
