static char rcsid[]="$Id: writeslice.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, write sliced binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsWriteSlice(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
{
   DDS_BIN *slice_bin, *former_bin;
   HOST_INT6_T new_offset;
   DDS_TYPE *detype;
   int n_write, n_bytes, n_want, n_chunk;
   const char *buf;

   errno = 0;
   buf = buffer;

   for(n_write = 0; n_write < count; n_write += n_chunk) {
      /* write data vector using cdds_write. */
      if(! bin->slice_toc) {
         /* Current slice exhausted.  Increment to next slice. */
         if(bin->slice_now == bin->slice_max - 1) {
            if(bin->cube.axis[bin->cube.rank].flag & DDS_AXIS_SLICE) {
               break;
            }
            bin->slice_now = 0;
         } else {
            bin->slice_now++;
         }
         bin->slice_toc = bin->slice_tic;
      }

      n_want = count - n_write;

      if(n_want * type->stroke > bin->slice_toc) {
         /* Multiple writes required to span slices. */
         if(HOST_INT6_MOD(bin->slice_toc, type->stroke)) {
            ddsError("write slice type, not aligned\n\t%s, ReelTrc= %lu\n",
               bin->media->title.ptr, (unsigned long)bin->ReelTrc);
            break;
         }
         n_want = bin->slice_toc / type->stroke;
      }

      if (bin->slice_now >= bin->slice_max) bin->slice_now = 0;

      slice_bin = bin->slice[bin->slice_now];

      /* Make sure slice has been positioned properly */
      if (slice_bin->state & DDS_BIN_DESEEK) {
         former_bin = bin->slice[(bin->slice_now ?
            bin->slice_now : bin->slice_max) - 1];
       
         /* Get the former position (byte offset on previous slice). */
         if(ggetpos(former_bin->media->fp, &new_offset)) {
            ddsError("deseek getpos, %s\n\t%s, LineTrc= %lu\n",
               strerror(errno), former_bin->media->title.ptr,
               (unsigned long)bin->LineTrc);
            return 0;
         }

         new_offset -= former_bin->media->trace_bias;
         if (bin->slice_now) new_offset -= bin->slice_tic;

         detype = (DDS_TYPE*)((DDS_SYM_TAG*)(bin->sym_tag.ptr))
            [DDS_STYPE_UNSIGNED1].sym;

         new_offset = ddsSeekType(slice_bin, detype, new_offset, SEEK_SET);

         if(new_offset < 0) {
            ddsError("deseek, %s\n\t%s, LineTrc= %lu\n",
               strerror(errno), bin->media->title.ptr,
               (unsigned long)bin->LineTrc);
            return 0;
         }

         slice_bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_DESEEK;
      } 

/********************************************/
/* "type->tag" buggy, if slice tags differ. */
/********************************************/
      n_chunk = cdds_write(slice_bin->tag, type->tag, buf, n_want);

      if(n_chunk != n_want) {
         n_write += n_chunk;
         break;
      }

      /* reduce byte count remaining in current slice. */
      n_bytes = n_chunk * type->stroke;
      bin->slice_toc -= n_bytes;
      buf += n_bytes;
   }

   return ddsWriteCheck(bin, type, buffer, count, n_write, 0);
}
