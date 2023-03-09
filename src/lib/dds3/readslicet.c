static char rcsid[]="$Id: readslicet.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, read and edit sliced trace binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsReadSliceT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
{
   DDS_BIN *slice_bin, *former_bin;
   HOST_INT6_T new_offset;
   DDS_TYPE *detype;
   int n_read, n_bytes;
   char *buf, *rbuf;
   const char *ebuf;

   if(HOST_INT6_MOD(bin->slice_toc, type->stroke) != 0) {
      ddsError("read slice trace, not aligned\n\t%s, ReelTrc= %lu\n",
            bin->media->title.ptr, (unsigned long)bin->ReelTrc);
      return 0;
   }

   errno = 0;
   buf = buffer;
   n_bytes = 0;   /* number of bytes in rbuf */

   for(n_read = 0; n_read < count; n_read++) {

      reject_trace:
      if(! n_bytes) {
         /* attempt to read another trace */
         rbuf = buf;
         /* read another trace using cdds_read. */
         if(bin->slice_toc == 0) {
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

         if (bin->slice_now >= bin->slice_max) return 0;
         
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

         if(1 != cdds_read(slice_bin->tag, 0, rbuf, 1)) {
            /* Something went wrong... */
            break;
         }

         /* reduce byte count remaining in current slice. */
         bin->slice_toc -= type->stroke;
         n_bytes = type->stroke;
      }

      /* Edit the input trace in rbuf. */
      /* This "(void*)" violates "const". */
      ebuf = ddsEditTrace(bin, type, rbuf, DDS_BIN_MSG_READ);

      if(ebuf == rbuf) {
         /* trace is accepted by the API */
         memcpy(buf, ebuf, n_bytes);
         n_bytes = 0;
      } else if(ebuf == bin->dead_trace) {
         /* pass a dead trace to the API */
         memcpy(buf, ebuf, n_bytes);

         /* Counters reflect the media (not API). */
         bin->LineTrc--;
         bin->ReelTrc--;
         bin->FileTrc--;
      } else if(! ebuf) {
         /* Discard trace and read another. */
         /* Counters reflect the media (not API). */
         bin->LineTrc++;
         bin->ReelTrc++;
         bin->FileTrc++;
         n_bytes = 0;
         goto reject_trace;
      } else {
         /* edit error, terminate API */
         count = n_read;
         break;
      }

      buf += type->stroke;
      ddsCubeInc(bin);
   }

   if(n_bytes) {
      /* un-read some input data */
      gunread(slice_bin->media->fp, rbuf, n_bytes);
   }

   return ddsReadCheck(bin, type, buffer, count, n_read, 0);
}
