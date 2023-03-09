static char rcsid[]="$Id: f_malloc.c 148 2010-04-22 17:22:32Z user $";
/*
RCS-t=DDS API, malloc memory with error detection.

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
HOST_FTN_FUNC(fdds_malloc, FDDS_MALLOC)(
   HOST_FTN_INT *size)
{
   DDS_API_INIT;

   return cdds_malloc((size_t)*size);
}
