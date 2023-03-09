static char rcsid[]="$Id: hdrdeffloat.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, default header float value from dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsHdrDefFloat(
   float *dict_hdr,
   float *bin_hdr,
   const char *defn,
   size_t count)
{
   size_t n;

   if(! cdds_scanf(defn, "")) {
      for(n = 0; n < count; n++) {
         if(1 != cdds_scanf("", "%g", dict_hdr + n)) break;
      }
      if(n < count)
         memset((void*)(dict_hdr + n), '\0',
            (count - n) * sizeof(float));
   } else {
      memset((void*)dict_hdr, '\0', count * sizeof(float));
   }
   memcpy((void*)bin_hdr, (const void*)dict_hdr, count * sizeof(float));

   return;
}
