static char rcsid[]="$Id: strsetn.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, set (malloc and initialize) min sized DDS_STRING.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsStrSetN(
   DDS_STRING *str,
   const char *from,
   size_t size)
{
   size_t new_size;

   new_size = DDS_STR_MIN * (1 + size / DDS_STR_MIN);
   str->ptr = (char*)ddsMalloc(new_size);
   str->max = new_size;

   memcpy(str->ptr, from, size);
   str->ptr[size]= '\0';
   return str;
}
