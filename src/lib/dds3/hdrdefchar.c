static char rcsid[]="$Id: hdrdefchar.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, default header char value from dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsHdrDefChar(
   char *dict_hdr,
   char *bin_hdr,
   const char *defn,
   size_t count,
   size_t prec)
{
   size_t n, max;
   const char *value;

   if(! cdds_scanf(defn, "")) {
      for(n = 0; n < count; n++) {
         if(0 > cdds_scant("", &value)) break;
         max = strlen(value);
         if(max < prec) {
            memcpy(dict_hdr + n * prec, value, max);
            memset(dict_hdr + n * prec + max, ' ', prec - max);
         } else {
            memcpy(dict_hdr + n * prec, value, prec);
         }
      }
      if(n < count)
         memset((void*)(dict_hdr + n * prec), ' ',
            (count - n) * prec);
   } else {
      memset((void*)dict_hdr, ' ', count * prec);
   }
   memcpy((void*)bin_hdr, (const void*)dict_hdr, count * prec);

   return;
}
