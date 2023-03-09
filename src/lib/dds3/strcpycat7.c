static char rcsid[]="$Id: strcpycat7.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, copy string and concatenate six more strings onto DDS_STRING.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsStrCpyCat7(
   DDS_STRING *str,
   const char *from,
   const char *from2,
   const char *from3,
   const char *from4,
   const char *from5,
   const char *from6,
   const char *from7)
{
   size_t cpy_len, str_size, new_size;
   size_t cat_len2, cat_len3, cat_len4, cat_len5;
   size_t cat_len6, cat_len7;
   char *ptr;

   cpy_len = strlen(from);
   cat_len2 = strlen(from2);
   cat_len3 = strlen(from3);
   cat_len4 = strlen(from4);
   cat_len5 = strlen(from5);
   cat_len6 = strlen(from6);
   cat_len7 = strlen(from7);

   str_size = cpy_len + cat_len2 + cat_len3 + cat_len4 + cat_len5
      + cat_len6 + cat_len7;

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
   memcpy(ptr, from4, cat_len4); ptr += cat_len4;
   memcpy(ptr, from5, cat_len5); ptr += cat_len5;
   memcpy(ptr, from6, cat_len6); ptr += cat_len6;
   memcpy(ptr, from7, cat_len7); ptr += cat_len7;
   *ptr = '\0';

   return str;
}
