static char rcsid[]="$Id: hdrmodchar.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, update header char value within dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsHdrModChar(
   const char *dict_hdr,
   const char *bin_hdr,
   const char *defn,
   size_t count,
   size_t prec)
{
   static DDS_STRING str = NULL_STRING;
   size_t n;

   if(memcmp((const void*)dict_hdr, (const void*)bin_hdr,
      count * prec)) {
      cdds_printf(defn, "");
      for(n = 0; n < count; n++) {
         ddsStrCpyNB(&str, bin_hdr + n * prec, prec);
         if(count > 1) cdds_printf("", "\n");
         cdds_printt("", str.ptr);
      }
      cdds_printf("", "\n");
   }
   return;
}
