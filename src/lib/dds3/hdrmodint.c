static char rcsid[]="$Id: hdrmodint.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, update header int value within dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsHdrModInt(
   const int *dict_hdr,
   const int *bin_hdr,
   const char *defn,
   size_t count)
{
   size_t n;

   if(memcmp((const void*)dict_hdr, (const void*)bin_hdr,
      count * sizeof(int))) {
      cdds_printf(defn, "");
      for(n = 0; n < count; n++) {
         if(count > 5 && ! (n % 10)) cdds_printf("", "\n");
         cdds_printf("", " %i", bin_hdr[n]);
      }
      cdds_printf("", "\n");
   }
   return;
}
