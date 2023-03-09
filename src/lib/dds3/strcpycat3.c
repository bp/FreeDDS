static char rcsid[]="$Id: strcpycat3.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, copy string and concatenate two more strings onto DDS_STRING.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsStrCpyCat3(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3)
{
   size_t cpy_len, str_size, new_size;
   size_t cat_len2, cat_len3;
   char *ptr;

   cpy_len = strlen(from);
   cat_len2 = strlen(from2);
   cat_len3 = strlen(from3);

   str_size = cpy_len + cat_len2 + cat_len3;

   if(str->ptr) {
      if(str->max <= str_size){
         new_size = DDS_STR_MIN * (1 + str_size / DDS_STR_MIN);
         str->ptr = (char*)ddsRealloc(str->ptr, new_size);
         str->max = new_size;
      }
   } else {
      new_size = DDS_STR_MIN * (1 + str_size / DDS_STR_MIN);
      str->ptr = (char*)ddsMalloc(new_size);
      str->max = new_size;
   }
      
   ptr = str->ptr;
   memcpy(ptr, from,  cpy_len);  ptr += cpy_len;
   memcpy(ptr, from2, cat_len2); ptr += cat_len2;
   memcpy(ptr, from3, cat_len3); ptr += cat_len3;
   *ptr = '\0';

   return str;
}
