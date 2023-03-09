static char rcsid[]="$Id: cubeinc.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, increment the cube sort indices to next expected value

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsCubeInc(
   DDS_BIN *bin)
{
   DDS_AXIS *axis;
   int n, rank, rank_max = bin->cube.rank;

   bin->LineTrc++;
   bin->ReelTrc++;
   bin->FileTrc++;

   if(bin->cube.io_rank > rank_max) return;

   for(rank = bin->cube.io_rank; rank <= rank_max; rank++) {
      /* Massage the next axis. */
      axis = bin->cube.axis + rank;

      /* Increment ordinal to the next expected value. */
      axis->ordinal++;

      if(axis->ordinal < axis->size ||
         rank == rank_max) {
         /* No carry to next axis. */
         break;
      } 

      /* Carry, increment next axis also. */
      axis->ordinal = 0;
      /* This axis is now synchronized (but may be re-cycled). */
      axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_CYCLE;
   }

   /* No carry to next axis. */
   if(axis->flag & DDS_AXIS_CYCLE) {
      /* This axis is still being synchronized, */
      /* which forces another cycle on lower axes. */
      for(n = bin->cube.io_rank; n < rank; n++) {
         axis = bin->cube.axis + n;
         if(axis->flag & DDS_AXIS_SYNC) {
            /* Re-cycle a lower axis. */
            axis->flag |= DDS_AXIS_CYCLE;
            axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_CHANGE;
         }
      }
   }

   for(n = rank + 1; n <= rank_max; n++) {
      axis = bin->cube.axis + n;
      /* Enable SYNC check in higher axis. */
      axis->flag |= DDS_AXIS_CHANGE;
   }

   return;
}
