static char rcsid[]="$Id: c_calloc.c 148 2010-04-22 17:22:32Z user $";
/*
RCS-t=DDS API, clear malloc memory with error detection.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void * (cdds_calloc)(
   size_t nobj,
   size_t size)
{
   DDS_API_ENTRY("calloc")

   DDS_API_EXIT(ddsZalloc(size * nobj));
}
