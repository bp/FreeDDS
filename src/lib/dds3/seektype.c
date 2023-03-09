static char rcsid[]="$Id: seektype.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, seek within binary data (non-sliced)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_INT6_T ddsSeekType(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   int whence)
{
   HOST_INT6_T new_offset;
   HOST_LLONG_T TrcOff;
   DDS_AXIS *axis;
   int rank, rank_max;

   switch(whence) {
   case SEEK_SET:
      new_offset = ddsSeekSet(bin, type, offset, &TrcOff);
      break;
   case SEEK_CUR:
      new_offset = ddsSeekCur(bin, type, offset, &TrcOff);
      break;
   case SEEK_END:
      new_offset = ddsSeekEnd(bin, type, offset, &TrcOff);
      break;
   }

   if(new_offset >= 0) {
      bin->LineTrc = bin->ReelTrc = bin->FileTrc = TrcOff + 1;

      rank_max = bin->cube.rank;

      /* Compute axis ordinals, given the trace offset. */
      for(rank = bin->cube.io_rank; rank <= rank_max; rank++) {
         axis = bin->cube.axis + rank;

         /* Force expected ordinal value within the SOLID cube. */
         if(rank < rank_max) {
            axis->ordinal = TrcOff % axis->size;
            TrcOff /= axis->size;
         } else {
            axis->ordinal = TrcOff;
         }
      }
   }

   return new_offset;
}
