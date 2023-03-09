static char rcsid[]="$Id: c_getff.c 196 2010-10-25 23:23:21Z user $";
/*
RCS-t=DDS API, get fake float(s) from consecutive 1,2 or 4-byte values
*/

/******************************************/
/***   Copyright (C) 2004               ***/
/***   BP AMERICA, Houston, TX          ***/
/***   Jerry Ehlers, 30 March 2004      ***/
/******************************************/

#include "dds_sys.h"


int cdds_getff(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const void *buf,
   float *array,
   int count)
{
   int   ier, i, i1, i2, swap=0;
   char  cbuf, *abuf = (char*)array;
   BIN_TAG sym2_tag;

   DDS_API_ENTRY("getff");

   if (sym_tag < -1) {
      sym_tag = -sym_tag;
      sym2_tag = sym_tag / 10000;
      sym_tag = sym_tag % 10000;
      if (sym2_tag == 1) {
         swap = 1;
      } else if (sym2_tag > 1) {
         /* 
          *   get fake float from general double header words
          *   (This can only work for a count of 1)
          */

         count = 0;
 
         ier = cdds_geti(bin_tag, sym_tag, buf, &i1, 1);
         if (ier != 1) {
            DDS_API_EXIT(count);
         }

         ier = cdds_geti(bin_tag, sym2_tag, buf, &i2, 1);
         if (ier != 1) {
            DDS_API_EXIT(count);
         }

#ifdef HOST_SWAP
         abuf[3] = (i1 >> 8) & 0xff;
         abuf[2] = i1 & 0xff;
         abuf[1] = (i2 >> 8) & 0xff;
         abuf[0] = i2 & 0xff;
#else
         abuf[0] = (i1 >> 8) & 0xff;
         abuf[1] = i1 & 0xff;
         abuf[2] = (i2 >> 8) & 0xff;
         abuf[3] = i2 & 0xff;
#endif
         count = 1;
         DDS_API_EXIT(count)
      }
#ifdef HOST_SWAP
   } else {
      swap = 1;
#endif
   }

   /* 
    *   get fake floats from pairs of 2-byte consecutive words
    */

   count *= 4;
   ier = cdds_get(bin_tag, sym_tag, buf, array, count);

#ifdef HOST_SWAP
   if (swap) {
      for (i=0;i<count;i+=4) {
         cbuf      = abuf[i];
         abuf[i]   = abuf[i+2];
         abuf[i+2] = cbuf;
         cbuf      = abuf[i+1];
         abuf[i+1] = abuf[i+3];
         abuf[i+3] = cbuf;
      }
   } else {
      for (i=0;i<count;i+=4) {
         cbuf      = abuf[i];
         abuf[i]   = abuf[i+3];
         abuf[i+3] = cbuf;
         cbuf      = abuf[i+1];
         abuf[i+1] = abuf[i+2];
         abuf[i+2] = cbuf;
/*
         cbuf      = abuf[i];
         abuf[i]   = abuf[i+1];
         abuf[i+1] = cbuf;
         cbuf      = abuf[i+2];
         abuf[i+2] = abuf[i+3];
         abuf[i+3] = cbuf;
*/
      }
   }
#else
   if (swap) {
      for (i=0;i<count;i+=4) {
         cbuf      = abuf[i];
         abuf[i]   = abuf[i+1];
         abuf[i+1] = cbuf;
         cbuf      = abuf[i+2];
         abuf[i+2] = abuf[i+3];
         abuf[i+3] = cbuf;
      }
   }
#endif

   count = ier/4;

   DDS_API_EXIT(count)
}
