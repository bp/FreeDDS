static char rcsid[]="$Id: strcat3.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, concatenate three strings onto DDS_STRING.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsStrCat3(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3)
{
   size_t old_len, cat_len, cat_len2, cat_len3;
   size_t cat_size, new_size;
   char *ptr;

   cat_len  = strlen(from);
   cat_len2 = strlen(from2);
   cat_len3 = strlen(from3);
   cat_size = cat_len + cat_len2 + cat_len3;

   if(str->ptr) {
      old_len = strlen(str->ptr);
      if(str->max <= old_len + cat_size) {
         new_size = DDS_STR_MIN * (1 + (old_len + cat_size) / DDS_STR_MIN);
         str->ptr = (char*)ddsRealloc(str->ptr, new_size);
         str->max = new_size;
      }
   } else {
      old_len = 0;
      new_size = DDS_STR_MIN * (1 + cat_size / DDS_STR_MIN);
      str->ptr = (char*)ddsMalloc(new_size);
      str->max = new_size;
   }
      
   ptr = str->ptr + old_len;
   memcpy(ptr, from,  cat_len);  ptr += cat_len;
   memcpy(ptr, from2, cat_len2); ptr += cat_len2;
   memcpy(ptr, from3, cat_len3); ptr += cat_len3;
   *ptr = '\0';

   return str;
}
