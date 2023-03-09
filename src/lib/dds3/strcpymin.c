static char rcsid[]="$Id: strcpymin.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, copy string into DDS_STRING, with minimum size.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsStrCpyMin(
   DDS_STRING *str,
   const char *from,
   size_t min)
{
   size_t size, min_size, new_size;

   size = strlen(from);
   if(size < min)
      min_size = min;
   else
      min_size = size;

   if(str->max <= min_size) {
      new_size = DDS_STR_MIN * (1 + min_size / DDS_STR_MIN);
      if(str->ptr)
         str->ptr = (char*)ddsRealloc(str->ptr, new_size);
      else
         str->ptr = (char*)ddsMalloc(new_size);
      str->max = new_size;
   }

   if(size) memcpy(str->ptr, from, size);
   str->ptr[size]= '\0';
   return str;
}
