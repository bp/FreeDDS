static char rcsid[]="$Id: cubelinker.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, link binary cube description

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsCubeLinker(
   DDS_BIN *bin)
{
   DDS_AXIS *axis, *lower;
   HOST_LLONG_T bytes, slice;
   int n, rank_max, io_rank;

   io_rank = bin->cube.io_rank;
   rank_max = bin->cube.rank;

   for(n = 1; n <= rank_max; n++, lower = axis) {
      axis = bin->cube.axis + n;

      if(! axis->size && n >= io_rank) {
         ddsError("forcing axis %i, size.%s= 1\n\tbinary= %s\n",
            n, axis->name, bin->name);
         axis->size = 1;
      }

      axis->ordinal = 0;

      if(n == 1 || n == io_rank) {
         /* Sub-cubes are solid by definition. */
         axis->flag |= DDS_AXIS_SOLID;
      } else if(lower->flag & DDS_AXIS_SOLID) {
         /* The solid attribute has propagated to the sub-cube. */
         if(lower->size == 1 || ! (lower->flag & DDS_AXIS_SPARSE)) {
            /* Propagate the solid attribute to another level. */
            axis->flag |= DDS_AXIS_SOLID;
         }
      }

      if(n < io_rank) {
         axis->stripe = 0;
         if(n == 1)
            axis->stride = 1;
         else
            axis->stride = lower->size * lower->stride;
      } else if(n == io_rank) {
         bytes = bin->trace.type->stroke;
         slice = 1;
         axis->stripe = 1;
         if(axis->flag & DDS_AXIS_SLICE) {
            axis->stride = slice;
            slice *= axis->size;
         } else {
            axis->stride = bytes;
            bytes *= axis->size;
         }
      } else {
         axis->stripe = lower->size * lower->stripe;
         if(axis->flag & DDS_AXIS_SLICE) {
            axis->stride = slice;
            slice *= axis->size;
         } else {
            axis->stride = bytes;
            bytes *= axis->size;
         }
      }

      if(axis->sort_name && *axis->sort_name != '?') {
         axis->flag |= DDS_AXIS_SORT;
      }

      if(1 < n &&
         n != io_rank &&
         (axis->flag & DDS_AXIS_SORT) &&
         ! (lower->flag & DDS_AXIS_SPARSE) &&    /* good ? bad ? ugly ? */
         ! (lower->flag & DDS_AXIS_ASYNC)) {
         /* this axis synchronizes the next lower axes */
         lower->flag |= (DDS_AXIS_FLAGS)(DDS_AXIS_SYNC);
      }

      if(n >= io_rank &&
         (axis->flag & DDS_AXIS_SORT) &&
         ! (axis->flag & DDS_AXIS_SPARSE) &&
         ! (axis->flag & (DDS_AXIS_FORCE | DDS_AXIS_REJECT))) {
         ddsVerbose("axis %i 'sort.%s= %s' i/o index errors may be issued\n"
            "\tspecify 'edit.%s= {force | reject}' to prevent trouble\n"
            "\tbinary= %s\n",
            n, axis->name, axis->sort_name,
            axis->name,
            bin->name);
      }
   }

   if(bin->media->state & DDS_MEDIA_SWAP) {
      ddsOpenEstimate(bin);
   }

   return;
}
