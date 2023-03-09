static char rcsid[]="$Id: editfinal.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, edit final binary trace data, for read or write

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void * ddsEditFinal(
   DDS_BIN *bin,
   DDS_TYPE *type,
   DDS_BIN_STATE msg_state)
{
   DDS_AXIS *axis;
   DDS_BOOLEAN more_dead = DDS_FALSE;
   DDS_COORD expect;
   int rank, rank_max;

   /***************************************************************/
   /* Return value is:                                            */
   /*    1) bin->dead_trace, iff a dead trace should be inserted. */
   /*    2) NULL, iff dead trace padding is done for the cube.    */
   /***************************************************************/

   rank_max = bin->cube.rank;

   for(rank = bin->cube.io_rank; rank <= rank_max; rank++) {
      /* edit the next axis */
      axis = bin->cube.axis + rank;

      if(axis->flag & DDS_AXIS_SORT) {
         /* This axis has a sort index, fix its value. */
         expect = ddsGetExpected(axis->ordinal, axis);
         CON_NASS(axis->sort.type->ptype, DDS_CTYPE_DOUBLE,
            bin->dead_trace + axis->sort.offset, &expect);
      }

      if(axis->ordinal &&
         rank < rank_max &&
         ((axis->flag & DDS_AXIS_PAD) ||
          ! (axis->flag & DDS_AXIS_SPARSE))) {
         /* More dead traces are needed. */
         more_dead = DDS_TRUE;
      }
   }

   if(! more_dead) {
      /* dead trace padding for the cube is done */
      return NULL;
   }

   /* insert one dead trace until synchronized */
   bin->count_dead++;

   if(bin->trace.r_seam.thread) {
      /* Evaluate map read expressions. */
      bin->trace.r_object->ptr = bin->dead_trace;
      RELOC_VALUE(bin->trace.r_object);
      ddsEvaluate(bin->trace.r_seam.thread);
   }

   return bin->dead_trace;
}
