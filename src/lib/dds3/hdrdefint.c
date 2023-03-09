static char rcsid[]="$Id: hdrdefint.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, default header int value from dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsHdrDefInt(
   int *dict_hdr,
   int *bin_hdr,
   const char *defn,
   size_t count)
{
   size_t n;

   if(! cdds_scanf(defn, "")) {
      for(n = 0; n < count; n++) {
         if(1 != cdds_scanf("", "%i", dict_hdr + n)) break;
      }
      if(n < count)
         memset((void*)(dict_hdr + n), '\0',
            (count - n) * sizeof(int));
   } else {
      memset((void*)dict_hdr, '\0', count * sizeof(int));
   }
   memcpy((void*)bin_hdr, (const void*)dict_hdr, count * sizeof(int));

   return;
}
