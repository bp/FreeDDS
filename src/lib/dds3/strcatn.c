static char rcsid[]="$Id: strcatn.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, concatenate sized strings onto DDS_STRING.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsStrCatN(
   DDS_STRING *str,
   const char *from,
   size_t size)
{
   size_t old_len, new_size;

   if(str->ptr) {
      old_len = strlen(str->ptr);
      if(str->max <= old_len + size) {
         new_size = DDS_STR_MIN * (1 + (old_len + size) / DDS_STR_MIN);
         str->ptr = (char*)ddsRealloc(str->ptr, new_size);
         str->max = new_size;
      }
   } else {
      old_len = 0;
      new_size = DDS_STR_MIN * (1 + size / DDS_STR_MIN);
      str->ptr = (char*)ddsMalloc(new_size);
      str->max = new_size;
   }
      
   memcpy(str->ptr + old_len, from, size);
   str->ptr[old_len + size]= '\0';
   return str;
}

