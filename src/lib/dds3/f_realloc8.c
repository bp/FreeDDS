static char rcsid[]="$Id: f_realloc8.c 289 2015-01-29 00:14:38Z user $";
/*
RCS-t=DDS API, realloc memory with error detection.

BUGS: the maximum number of bytes that can be allocated
in one call is limited to HOST_FTN_INT max value.
This is less than size_t (actual limit of malloc func).
On the SGI in 64 bit mode, this max is 2**31 - 1.

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
HOST_FTN_FUNC(fdds_realloc8, FDDS_REALLOC8)(
   HOST_FTN_PTR *ptr,
   HOST_FTN_DBL *size8)
{
   HOST_FTN_DBL tmp8;
   size_t size;

   DDS_API_INIT;

   size = *size8;
   tmp8 = size;
   if (tmp8 != *size8) {
      ddsPanic("(fdds_realloc8) system cannot handle this size (%lg != %lg)\n",
         *size8, tmp8);
   }

   return cdds_realloc((void*)*ptr, size);
}
