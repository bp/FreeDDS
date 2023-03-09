static char rcsid[]="$Id: ftncpy.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, copy string into fortran character array (pad blanks)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

char * ddsFtnCpy(
   char *array,
   size_t size,
   const char *str)
{
   size_t max;

   if (!str) {
      memset(array, ' ', size);
   }
   else if ((max = strlen(str)) < size) {
      memcpy(array, str, max);
      memset(array + max, ' ', size - max);
   } else {
      memcpy(array, str, size);
   }

   return array;
}
