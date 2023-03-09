static char rcsid[]="$Id: writeslicet.c 227 2011-05-04 20:08:56Z user $";
/*
RCS-t=DDS API, edit and write sliced trace binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsWriteSliceT(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const char *buffer,
   int count)
{
   DDS_BIN *slice_bin, *former_bin;
   HOST_INT6_T new_offset;
   DDS_TYPE *detype;
   int n_write, n_bytes;
   const char *buf;
   char *ebuf;

   if(HOST_INT6_MOD(bin->slice_toc, type->stroke)) {
      ddsError("write slice trace, not aligned\n\t%s, ReelTrc= %lu\n",
            bin->media->title.ptr, (unsigned long)bin->ReelTrc);
      return 0;
   }

   errno = 0;
   buf = buffer;

   for(n_write = 0; n_write < count; n_write++) {

      /* Edit the output trace in buf. */
      /* This "(void*)" violates "const". */
      pad_dead:
      ebuf = ddsEditTrace(bin, type, (void*)buf, DDS_BIN_MSG_WRITE);

      if(ebuf == buf || ebuf == bin->dead_trace) {
         /* write another trace using cdds_write. */
         if(! bin->slice_toc) {
            /* Current slice exhausted.  Increment to next slice. */
            if(bin->slice_now == bin->slice_max - 1) {
               /* jwe May 2011: Not sure what this is here for, but it prevents slicing at times
               if(bin->cube.axis[bin->cube.rank].flag & DDS_AXIS_SLICE) {
                  break;
               }
               */
               bin->slice_now = 0;
            } else {
               bin->slice_now++;
            }
            bin->slice_toc = bin->slice_tic;
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

         if(1 == cdds_write(slice_bin->tag, 0, ebuf, 1)) {
            /* reduce byte count remaining in current slice. */
            bin->slice_toc -= type->stroke;
         } else {
            /* Something went wrong... */
            break;
         }

         n_bytes = type->stroke;
      }

      if(ebuf == buf) {
         /* trace is accepted by the API */
         if(bin->state & DDS_BIN_PADDING) {
            /* Just kidding; close is padding dead traces. */
            count = n_write;
            break;
         }
      } else if(ebuf == bin->dead_trace) {
         /* write a dead trace before the API trace */
         ddsCubeInc(bin);
         goto pad_dead;
      } else if(! ebuf) {
         /* discard API trace and edit the next one */
         buf += type->stroke;
         continue;
      } else {
         /* edit error, terminate API */
         count = n_write;
         break;
      }

      buf += n_bytes;
      ddsCubeInc(bin);
   }

   return ddsWriteCheck(bin, type, buffer, count, n_write, 0);
}
