static char rcsid[]="$Id: seekslice.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, seek within sliced binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_INT6_T ddsSeekSlice(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   int whence)
{
   DDS_BIN *slice_bin, *former_bin;
   HOST_INT6_T new_offset, tmp_offset, sub_offset;
   HOST_LLONG_T TrcOff;
   DDS_AXIS *axis;
   DDS_BOOLEAN click;
   DDS_MEDIA *slice_media;
   int n, fd, rank, rank_max, slices;

   rank_max = bin->cube.rank;

   offset *= type->prec;

   switch(whence) {
   case SEEK_SET:
      new_offset = offset;
      break;
   case SEEK_CUR:
      /* readjust slice pointers. */
      if(bin->slice_toc == 0) {
         /* Current slice exhausted.  Increment to next slice. */
         if(bin->slice_now == bin->slice_max - 1) {
            if(!(bin->cube.axis[bin->cube.rank].flag & DDS_AXIS_SLICE)) {
               bin->slice_now = 0;
            }
            /* end of data */
            else {
               slice_bin = bin->slice[bin->slice_max - 1];
               fd = gfileno(slice_bin->media->fp);
               new_offset = offset + bin->slice_max 
                  * (ddsFstatSize(fd) - slice_bin->media->trace_bias);
               break;
            }
         } else {
            bin->slice_now++;
         }
         bin->slice_toc = bin->slice_tic;
      }
 
      if(bin->slice_now >= bin->slice_max) {
         ddsError("seek beyond the last slice\n\t%s, LineTrc= %lu\n",
            bin->media->title.ptr, (unsigned long)bin->LineTrc);
         return DDS_BAD;
      }

      slice_bin = bin->slice[bin->slice_now];
      slice_media = slice_bin->media;

      /* Make sure slice has been positioned properly */
      if (slice_bin->state & DDS_BIN_DESEEK) {
         former_bin = bin->slice[(bin->slice_now ?
            bin->slice_now : bin->slice_max) - 1];
         
         /* Get the former position (byte offset on previous slice). */
         if(ggetpos(former_bin->media->fp, &tmp_offset)) {
            ddsError("deseek getpos, %s\n\t%s, LineTrc= %lu\n",
               strerror(errno), former_bin->media->title.ptr,
               (unsigned long)bin->LineTrc);
            return 0;
         }

         tmp_offset += slice_bin->media->trace_bias - former_bin->media->trace_bias;
         if (bin->slice_now) tmp_offset -= bin->slice_tic;
      } else if(ggetpos(slice_media->fp, &tmp_offset)) {
         ddsError("seek getpos, %s\n\t%s, LineTrc= %lu\n",
            strerror(errno), slice_media->title.ptr,
            (unsigned long)bin->LineTrc);
         return DDS_BAD;
      }

      tmp_offset -= slice_media->trace_bias;

      assert(tmp_offset >= 0);
      
      /* Initialize new_offset to 0. */
      /* Its incremented as needed for the trace offset within a slice. */
      new_offset = 0;

      TrcOff = HOST_INT6_DIV(tmp_offset, bin->trace.type->prec);

      for(rank = bin->cube.io_rank; rank <= rank_max; rank++) {
         axis = bin->cube.axis + rank;

         if(axis->flag & DDS_AXIS_SLICE) {
            axis->ordinal = bin->slice_now % axis->size;
            bin->slice_now /= axis->size;
         } else {
            if(rank < rank_max) {
               axis->ordinal = TrcOff % axis->size;
               TrcOff /= axis->size;
            } else {
               axis->ordinal = TrcOff;
            }
         }
         new_offset += axis->ordinal * axis->stripe;
      }

      /* Change new_offset to the byte offset within a trace. */
      new_offset = new_offset * bin->trace.type->prec 
         + HOST_INT6_MOD(tmp_offset, bin->trace.type->prec);

      new_offset += offset;
      break;
   case SEEK_END:
      slices = bin->slice_max;
      slice_bin = bin->slice[bin->slice_max - 1];
      while(slice_bin->slice) {
         slices *= slice_bin->slice_max;
         slice_bin = slice_bin->slice[slice_bin->slice_max-1];
      }
      fd = gfileno(slice_bin->media->fp);
      new_offset = offset +
         slices * (ddsFstatSize(fd) - slice_bin->media->trace_bias);
      break;
   }

   /* new_offset is the new byte offset within the trace sequence. */

   if(new_offset < 0) {
      ddsError("seek to negative offset\n\t%s, LineTrc= %lu\n",
         bin->media->title.ptr, (unsigned long)bin->LineTrc);
      errno = EINVAL;
      return DDS_BAD;
   }

   TrcOff = HOST_INT6_DIV(new_offset, bin->trace.type->prec);
   bin->LineTrc = bin->ReelTrc = bin->FileTrc = TrcOff + 1;

   /* click is true, after the first axis is sliced. */
   click = DDS_FALSE;

   /* Initialize sub_offset to the byte offset within a trace. */
   /* Its incremented as needed for the byte offset within a slice. */
   sub_offset = HOST_INT6_MOD(new_offset, bin->trace.type->prec);

   new_offset = HOST_INT6_DIV(new_offset, type->prec);

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

      if(axis->flag & DDS_AXIS_SLICE) {
         if(click) {
            /* Another axis is sliced. */
            bin->slice_now += axis->ordinal * axis->stride;
         } else {
            /* This is the first axis that is sliced. */
            click = DDS_TRUE;
            bin->slice_now = axis->ordinal * axis->stride;
         }
      } else {
         sub_offset += axis->ordinal * axis->stride;
      }
   }

   /* Set the delayed seek flag in all slices. */
   for(n = 0; n < bin->slice_max; n++) {
      slice_bin = bin->slice[n];
      slice_bin->state |= DDS_BIN_DESEEK;
   }

   if(bin->slice_now == bin->slice_max && ! sub_offset) {
      /* special case when seeking to the end. */
      bin->slice_now--;
      bin->slice_toc = 0;
      sub_offset = bin->slice_tic;
   } else {
      /* bin->slice_toc = bin->slice_tic - sub_offset; */
      bin->slice_toc = bin->slice_tic - HOST_INT6_MOD(sub_offset, bin->slice_tic);
      if (bin->slice_toc <= 0) {
         bin->slice_toc = bin->slice_tic;
      } 
   }

   if(bin->slice_now < bin->slice_max) {
      slice_bin = bin->slice[bin->slice_now];

      /* Execute the first seek within the current slice. */
      cdds_lseek8(slice_bin->tag, DDS_CHAR, sub_offset, SEEK_SET);
   }

   return new_offset;
}
