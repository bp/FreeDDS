static char rcsid[]="$Id: edittrace.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, edit binary trace data, for read or write

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "usp_hdr.h"
#include "segy_hdr.h"

long ddsGetOrdinal(DDS_COORD coord, DDS_AXIS *axis)
{
   int i, j;
   long ordinal;
   double max, x;

   if (! axis->ngrid) {
      ordinal = (long)(1.5+(coord-axis->base)/axis->step)-1;
   } else {
      j = 1;
      max = (axis->grid[1]-coord)*(coord - axis->grid[0]);
      for(i=2;i<axis->ngrid;i++) {
         x = (axis->grid[i]-coord)*(coord - axis->grid[i-1]);
         if (x < max) break;
         j = i;
         max = x;
      }
      ordinal = (long)(1.5+j-1+(coord-axis->grid[j-1])/
                          (axis->grid[j]-axis->grid[j-1]))-1;
   }

   return(ordinal);
}

DDS_COORD ddsGetExpected(long ordinal, DDS_AXIS *axis)
{
   DDS_COORD expect;

   if (! axis->ngrid) {
      expect = axis->base + ordinal * axis->step;
   } else {
      if (ordinal < 0) {
         expect = axis->grid[0] + ordinal * (axis->grid[1]-axis->grid[0]);
      } else if (ordinal >= axis->ngrid) {
         expect = axis->grid[axis->ngrid-1] + (ordinal-axis->ngrid+1) 
            * (axis->grid[axis->ngrid-1]-axis->grid[axis->ngrid-2]);
      } else {
         expect = axis->grid[ordinal];
      }
   }

   return(expect);
}


void * ddsEditTrace(
   DDS_BIN *bin,
   DDS_TYPE *type,
   void *buffer,
   DDS_BIN_STATE msg_state)
{
   DDS_MEDIA *media;
   DDS_AXIS *prev_axis, *next_axis, *axis, *tmp_axis;
   DDS_BOOLEAN error, warn, force, dead, fail;
   DDS_COORD coord, coord2, expect, expect_min, expect_max;
   int j, io_rank, rank, rank_max, TrcType, sync_limit;
   long ordinal;
   char *buf;

   /******************************************************************/
   /* buffer contains the read or write trace to be editted.         */
   /* Return value is:                                               */
   /*    1) buffer, iff the trace is accepted for processing.        */
   /*    2) bin->dead_trace, iff a dead trace should be inserted.    */
   /*    3) NULL, iff the trace should be discarded.                 */
   /*    4) DDS_BAD_PTR, iff an error was issued (terminate API).    */
   /******************************************************************/

   media = bin->media;
   io_rank = bin->cube.io_rank;
   rank_max = bin->cube.rank;
   buf = buffer;

   if(msg_state == DDS_BIN_MSG_READ) {
      if(bin->trace.r_seam.thread) {
         /* Evaluate map read expressions. */
         bin->trace.r_object->ptr = buf;
         RELOC_VALUE(bin->trace.r_object);
         ddsEvaluate(bin->trace.r_seam.thread);
      }
   }

   if(! (bin->state & DDS_BIN_PADDING)) {
      /*****************************************************/
      /* Gross editting based upon TrcType and sort index. */
      /* This is used to reject traces that should not     */
      /* participate in sort index synchronization.        */
      /* For example, dead traces or bogus sort indices.   */
      /*****************************************************/

      switch(bin->fmt) {
      case DDS_FMT_USP:
         CON_ANSS(DDS_CTYPE_INT, bin->TrcType.type->ptype,
            &TrcType,  buf + bin->TrcType.offset);

         if(((bin->state & DDS_drop_dead) && TrcType == USP_TRACE_DEAD) ||
            ((bin->state & DDS_drop_live) && TrcType != USP_TRACE_DEAD)) {
            bin->count_gross++;
            return NULL;
         } else if(TrcType == USP_TRACE_DEAD && (bin->state & DDS_zero_dead)) {
            memset((void*)(buf + bin->Samples.member->offset), '\0',
               bin->Samples.type->stroke);
         }
         break;
      case DDS_FMT_SEGY:
      case DDS_FMT_SEGY1:
      case DDS_FMT_SU:
         CON_ANSS(DDS_CTYPE_INT, bin->TrcType.type->ptype,
            &TrcType, buf + bin->TrcType.offset);

         if(((bin->state & DDS_drop_dead) && TrcType == SEGY_TRACE_DEAD) ||
            ((bin->state & DDS_drop_live) && TrcType != SEGY_TRACE_DEAD) ||
            ((bin->state & DDS_drop_aux)  && TrcType != SEGY_TRACE_DEAD &&
              TrcType != SEGY_TRACE_DATA)) {
            bin->count_gross++;
            return NULL;
         } else if(TrcType == SEGY_TRACE_DEAD && (bin->state & DDS_zero_dead)) {
            memset((void*)(buf + bin->Samples.member->offset), '\0',
               bin->Samples.type->stroke);
         }
         break;
      }

      /* Now check sort index ranges. */
      for(rank=io_rank;rank<=rank_max;rank++) {
         /* edit the next axis */
         axis = bin->cube.axis + rank;

         if(axis->flag & DDS_AXIS_SORT) {
            /* this axis has a sort index, retrieve its values */
            CON_ANSS(DDS_CTYPE_DOUBLE, axis->sort.type->ptype,
               &coord, buf + axis->sort.offset);

            /* scale index to an ordinal, 0 <= value < size */
            ordinal = ddsGetOrdinal(coord, axis);

            if(ordinal >= axis->size) {
               /* ordinal is greater than nominal window */
               if(! (axis->flag & DDS_AXIS_UPPER)) {
                  bin->count_gross++;
                  return NULL;
               }
            } else if(ordinal < 0) {
               /* ordinal is less than nominal window */
               if(! (axis->flag & DDS_AXIS_LOWER)) {
                  bin->count_gross++;
                  return NULL;
               }
            } else {
               /* ordinal is within nominal window */
               if(! (axis->flag & DDS_AXIS_WINDOW)) {
                  bin->count_gross++;
                  return NULL;
               }
            }
         }
      }
   }

   /*******************************************************/
   /* Synchronize actual and expected trace header values */
   /*******************************************************/

   if(bin->state & DDS_BIN_SOUGHT) {
      /* Previous i/o request was a seek. */
      bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_SOUGHT;

      for(rank = io_rank; rank <= rank_max; rank++) {
         axis = bin->cube.axis + rank;

         if(axis->flag & DDS_AXIS_SOLID) {
            /* ddsSeekTrace set the expected value. */
            continue;
         }

         if(axis->flag & DDS_AXIS_SORT) {
            /* This axis has a sort index, retrieve its values. */
            CON_ANSS(DDS_CTYPE_DOUBLE, axis->sort.type->ptype,
               &coord, buf + axis->sort.offset);

            /* scale index to an ordinal, 0 <= value < size */
            ordinal = ddsGetOrdinal(coord, axis);

            /* Force expected value to buf value. */
            axis->ordinal = ordinal % axis->size;
            axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_CHANGE;
         }
      }
   }

   sync_limit = 2 * rank_max + 1;

   recycle:   /* An upper axis change may induce lower axis cycles. */
   error = warn = force = dead = fail = DDS_FALSE;

   assert(sync_limit && "trace editting sync error");

   for(rank=io_rank;rank<=rank_max;rank++) {
      /* Edit the next axis. */
      axis = bin->cube.axis + rank;

      /* Assume this sort index will not have to be forced. */
      axis->flag &= (DDS_AXIS_FLAGS) ~ (DDS_AXIS_FIXED);

      if((axis->flag & (DDS_AXIS_SYNC | DDS_AXIS_CHANGE)) ==
         (DDS_AXIS_SYNC | DDS_AXIS_CHANGE)) {
         /*********************************************************/
         /* Synchronize sort index with changes in next axis,     */
         /* iff this axis is not already being synchronized.      */
         /*                                                       */
         /* For example, if RecNum in buf is different than       */
         /* the previous trace then CYCLE (pad) all lower axes.   */
         /* This circumvents problems with ambiguous TrcNum values*/
         /* when padding dead traces at a records end.            */
         /*********************************************************/

         assert(rank < rank_max && "termination constraint");

         next_axis = axis + 1;
         CON_ANSS(DDS_CTYPE_DOUBLE, next_axis->sort.type->ptype,
            &coord2, buf + next_axis->sort.offset);

         /* scale index to an ordinal, 0 <= value < size */
         ordinal = ddsGetOrdinal(coord2, axis);

         if(ordinal != next_axis->ordinal) {
            /* The index for the higher axis has changed. */
            /* Finish the cycle for this axis and all lower ones. */
            /* This will pad dead traces, if any are missing. */
            for(j = io_rank; j <= rank; j++) {
               tmp_axis = bin->cube.axis + j;

               if(tmp_axis->flag & DDS_AXIS_SYNC) {
                  /* Found an axis that needs synchronizing. */
                  tmp_axis->flag |= DDS_AXIS_CYCLE;
               }
            }

            /* Re-synchronize all lower axes. */
            goto recycle;
         }
      }

      if(axis->flag & DDS_AXIS_SORT) {
         /* This axis has a sort index, retrieve its values. */
         CON_ANSS(DDS_CTYPE_DOUBLE, axis->sort.type->ptype,
            &coord, buf + axis->sort.offset);

         /* scale index to an ordinal, 0 <= value < size */
         ordinal = ddsGetOrdinal(coord, axis);

         if(ordinal != axis->ordinal ||
            (axis->flag & DDS_AXIS_CYCLE)) {
            /******************************************************/
            /* Axis ordinals differs from the expected API value  */
            /* or it is participating in a synchronization cycle. */
            /*                                                    */
            /* Now we have to decide what to do about it,         */
            /* based upon flags associated with each axis.        */
            /*    ERROR  ? issue error and terminate API call.    */
            /*    WARN   ? issue warning and continue processing. */
            /*    FORCE  ? force the buf index to the expected.   */
            /*    REJECT ? discard this trace and try the next.   */
            /*    PAD    ? insert dead traces for missing ones.   */
            /*       If not FORCE, REJECT or PAD, then retain the */
            /*       buf index and change the expect API value.   */
            /******************************************************/

            if(axis->flag & DDS_AXIS_ERROR) {
               error = DDS_TRUE;
               break;
            } else if(axis->flag & DDS_AXIS_WARN) {
               warn = DDS_TRUE;
            }

            if(axis->flag & DDS_AXIS_FORCE) {
               /* Force sort index to expected value. */
               force = DDS_TRUE;
               axis->flag |= DDS_AXIS_FIXED;
            } else if(axis->flag & DDS_AXIS_REJECT) {
               /* Discard this input trace and edit another. */
               bin->count_reject++;
               return NULL;
            } else if(! (axis->flag & DDS_AXIS_SPARSE) ||
                ((axis->flag & DDS_AXIS_PAD) &&
                  msg_state == DDS_BIN_MSG_READ)) {
               if(ordinal < 0 || ordinal >= axis->size) {
                  /* Sort index falls outside the cube, */
                  /* padding has failed (impossible/meaningless). */
                  /* Blindly pass along the bogus index and warning. */
                  fail = DDS_TRUE;
                  expect = ddsGetExpected(axis->ordinal, axis);
                  expect_max = ddsGetExpected((axis->size-1), axis);
                  if (! axis->ngrid) {
                     expect_min = axis->base;
                  } else {
                     expect_min = axis->grid[0];
                  }
                  ddsWarn("%s trace %s axis %s= %g, expected= %g (%g to %g)\n"
                     "\t%s ReelTrc= %lu\n",
                     ((msg_state == DDS_BIN_MSG_READ) ? "read" : "write"),
                     axis->name, axis->sort_name, coord,
                     expect, expect_min, expect_max,
                     media->title.ptr, (unsigned long)bin->ReelTrc);

                  continue;
               }

               /* Insert one dead trace until synchronized. */
               warn = force = fail = DDS_FALSE;
               dead = DDS_TRUE;

               /* Fix all sort indices. */
               for(j=io_rank;j<=rank_max;j++) {
                  axis = bin->cube.axis + j;
                  if(axis->flag & DDS_AXIS_SORT) {
                     /* This axis has a sort index. */
                     if(j > rank) {
                        /* Check higher sort indices for errors. */
                        CON_ANSS(DDS_CTYPE_DOUBLE, axis->sort.type->ptype,
                           &coord, buf + axis->sort.offset);

                        /* scale index to an ordinal, 0 <= value < size */
                        ordinal = ddsGetOrdinal(coord, axis);

                        if(ordinal != axis->ordinal &&
                           (axis->flag & DDS_AXIS_ERROR)) {
                           /* Higher sort index implies ERROR. */
                           error = DDS_TRUE;
                           break;
                        }
                     }

                     /* Fix this sort index in the dead trace. */
                     force = DDS_TRUE;
                     axis->flag |= DDS_AXIS_FIXED;
                  }
               }

               /* Force expected ordinal as needed. */
               for(j=io_rank;j<=rank_max;j++) {
                  axis = bin->cube.axis + j;
                  if((axis->flag & DDS_AXIS_SORT) &&
                     (j <= rank || ! prev_axis->ordinal)) {
                     /* This axis has a sort index and */
                     /* the ordinal may need to be forced. */
                     if((axis->flag & DDS_AXIS_SPARSE) &&
                        (! (axis->flag & DDS_AXIS_PAD) ||
                          msg_state == DDS_BIN_MSG_WRITE)) {
                        /* This axis is SPARSE and */
                        /* this is NOT a PAD for a read or */
                        /* this is a WRITE, therefore */
                        /* force expected value to buf value. */
                        CON_ANSS(DDS_CTYPE_DOUBLE, axis->sort.type->ptype,
                           &coord, buf + axis->sort.offset);

                        /* scale index to an ordinal, 0 <= value < size */
                        ordinal = ddsGetOrdinal(coord, axis);

                        axis->ordinal = ordinal % axis->size;
                     }
                  }
                  prev_axis = axis;
               }

               /* break out of sync loop and deliver dead trace */
               buf = bin->dead_trace;

               break;
            } else {
               /* Force expected value to buf value. */
               axis->ordinal = ordinal % axis->size;
               axis->flag &= (DDS_AXIS_FLAGS) ~ DDS_AXIS_CHANGE;
            }
         }
      }
   }

   if(error) {
      ddsBinMessage(bin, msg_state);
      expect = ddsGetExpected(axis->ordinal, axis);
      ddsError("edit trace %s axis %s= %g, expected= %g\n"
         "\t ReelTrc= %lu\n",
         axis->name, axis->sort_name, coord, expect,
         media->title.ptr, (unsigned long)bin->ReelTrc);
      /* the error message may not abort processing */
      return DDS_BAD_PTR;
   }

   if(bin->count_dead && ! dead && ! (dds_state & DDS_DUMP_DATA)) {
      /* End of a dead sequence and data is not being dumped. */
      ddsBinMessage(bin, msg_state);
   }

   if(warn) bin->count_warn++;
   if(dead) bin->count_dead++;
   if(fail) bin->count_fail++;

   if(force) {
      /* Force/fix sort index values. */
      if((void*)buf == buffer) {
         /* Only count fixes on non-dead traces. */
         bin->count_force++;
      }

      for(rank=io_rank;rank<=rank_max;rank++) {
         /* Massage the next axis. */
         axis = bin->cube.axis + rank;

         if(axis->flag & DDS_AXIS_FIXED) {
            /* Force the sort index to the expected value. */
            expect = ddsGetExpected(axis->ordinal, axis);
            CON_NASS(axis->sort.type->ptype, DDS_CTYPE_DOUBLE,
               buf + axis->sort.offset, &expect);
         }
      }
   }

   if(msg_state != DDS_BIN_MSG_READ) {
      if(bin->trace.w_seam.thread) {
         /* Evaluate map write expressions. */
         bin->trace.w_object->ptr = buf;
         RELOC_VALUE(bin->trace.w_object);
         ddsEvaluate(bin->trace.w_seam.thread);
      }
   }

   return buf;
}
