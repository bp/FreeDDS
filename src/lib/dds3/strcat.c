static char rcsid[]="$Id: strcat.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, concatenate one string onto DDS_STRING.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsStrCat(
   DDS_STRING *str,
   const char *from)
{
   size_t old_len, cat_len;
   size_t new_size;
   char *ptr;

   cat_len  = strlen(from);

   if(str->ptr) {
      old_len = strlen(str->ptr);
      if(str->max <= old_len + cat_len) {
         new_size = DDS_STR_MIN * (1 + (old_len + cat_len) / DDS_STR_MIN);
         str->ptr = (char*)ddsRealloc(str->ptr, new_size);
         str->max = new_size;
      }
   } else {
      old_len = 0;
      new_size = DDS_STR_MIN * (1 + cat_len / DDS_STR_MIN);
      str->ptr = (char*)ddsMalloc(new_size);
      str->max = new_size;
   }
      
   ptr = str->ptr + old_len;
   memcpy(ptr, from,  cat_len);  ptr += cat_len;
   *ptr = '\0';

   return str;
}
