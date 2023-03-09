static char rcsid[]="$Id: c_putff.c 196 2010-10-25 23:23:21Z user $";
/*
RCS-t=DDS API, put fake float(s) into consecutive 1,2 or 4-byte values
*/

/******************************************/
/***   Copyright (C) 2004               ***/
/***   BP AMERICA, Houston, TX          ***/
/***   Jerry Ehlers, 30 March 2004      ***/
/******************************************/

#include "dds_sys.h"


int cdds_putff(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   void *buf,
   const float *array,
   int count)
{
   int   ier, i, i1, i2, swap=0;
   unsigned char  cbuf, *abuf = (unsigned char*)array;
   BIN_TAG sym2_tag;

   DDS_API_ENTRY("putff");

   if (sym_tag < -1) {
      sym_tag = -sym_tag;
      sym2_tag = sym_tag / 10000;
      sym_tag = sym_tag % 10000;
      if (sym2_tag == 1) {
         swap = 1;
      } else if (sym2_tag > 1) {
         /* 
          *   put fake float into general double header words
          *   (This can only work for a count of 1)
          */

         count = 0;

#ifdef HOST_SWAP
         i1 = (int)abuf[3] << 8 + (int)abuf[2];
         i2 = (int)abuf[1] << 8 + (int)abuf[0];
#else
         i1 = (int)abuf[0] << 8 + (int)abuf[1];
         i2 = (int)abuf[2] << 8 + (int)abuf[3];
#endif
         ier = cdds_puti(bin_tag, sym_tag, buf, &i1, 1);
         if (ier != 1) {
            DDS_API_EXIT(count);
         }

         ier = cdds_puti(bin_tag, sym2_tag, buf, &i2, 1);
         if (ier != 1) {
            DDS_API_EXIT(count);
         }
         count = 1;
         DDS_API_EXIT(count)
      }

#ifdef HOST_SWAP
   } else {
      swap = 1;
#endif
   }

   /* 
    *   put fake floats intp pairs of 2-byte consecutive words
    */

   count *= 4;

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

   ier = cdds_put(bin_tag, sym_tag, buf, array, count);
   count = ier/4;
   
   DDS_API_EXIT(count);
}
