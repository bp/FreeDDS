static char rcsid[]="$Id: c_realloc.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, realloc memory with error detection.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void * (cdds_realloc)(
   void *ptr,
   size_t size)
{
   DDS_API_ENTRY("realloc")
   DDS_API_EXIT(ddsRealloc(ptr, size))
}
