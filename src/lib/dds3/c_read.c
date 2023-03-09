static char rcsid[]="$Id: c_read.c 229 2011-06-02 18:53:57Z user $";
/*
RCS-t=DDS API, read binary data

*/

/**********************************************/
/***   Copyright (C) 1996,2004              ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_swap.h"

int cdds_read(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   void *buffer,
   int count)
{
   DDS_BIN *bin, *master_bin, *former_bin;
   DDS_TYPE *type, *detype;
   HOST_INT6_T new_offset;
   size_t reset;
   int n_read, cache;
   char *rbuf;
   /* This are for John's special I/O */
   int swap=0; /* for STREAM_TO_SYM_TAG & ddsStreamReadSwap */

   DDS_API_ENTRY("read")

   /*****************************/
   /* validate input parameters */
   /*****************************/

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(0));

   STREAM_TO_SYM_TAG(sym_tag, swap);

   SYM_TAG_TO_TYPE(type, bin, sym_tag, DDS_API_EXIT(0));

   NON_NEGATIVE(count, bin, DDS_API_EXIT(0));

   NON_NULL(buffer, bin, DDS_API_EXIT(0));

   /*****************************/
   /* Keep tabs on Samples read */
   /*****************************/

   dds_samples_in += count *  bin->cube.axis[1].size;

   if(bin->state & (DDS_BIN_FREEZE | DDS_TRACE_WRITE |
      DDS_BIN_DESEEK | DDS_BIN_EOF | DDS_BIN_EOM |
      DDS_BIN_MSG_WRITE | DDS_BIN_MSG_SEEK)) {
      /*****************************/
      /* read exception processing */
      /*****************************/

      /*********************************************************/
      /* Caution: bin->media will change, if media is mounted. */
      /*********************************************************/

      if(bin->state & DDS_TRACE_WRITE) {
         ddsPanic("read with DDS_TRACE_WRITE set\n");
      }

      if(bin->state & DDS_BIN_DESEEK) {
         /* Delayed seek pending for this slice. */
         master_bin = bin->master;
         former_bin = master_bin->slice[(master_bin->slice_now ?
            master_bin->slice_now : master_bin->slice_max) - 1];

         /* Get the former position (byte offset on previous slice). */
         if(ggetpos(former_bin->media->fp, &new_offset)) {
            ddsError("deseek getpos, %s\n\t%s, LineTrc= %lu\n",
               strerror(errno), former_bin->media->title.ptr,
               (unsigned long)master_bin->LineTrc);
            return 0;
         }

         new_offset += bin->media->trace_bias - former_bin->media->trace_bias;
         if (master_bin->slice_now) new_offset -= master_bin->slice_tic;

         detype = (DDS_TYPE*)((DDS_SYM_TAG*)(bin->sym_tag.ptr))
            [DDS_STYPE_UNSIGNED1].sym;

         new_offset = ddsSeekType(bin, detype, new_offset, SEEK_SET);

         if(new_offset < 0) {
            ddsError("deseek, %s\n\t%s, LineTrc= %lu\n",
               strerror(errno), bin->media->title.ptr,
               (unsigned long)bin->LineTrc);
            return 0;
         }

         bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_DESEEK;
      }

      if((bin->state & DDS_BIN_PADDING) &&
         type == bin->trace.type) {
         /* Final padding of dead traces. */
         errno = 0;
         n_read = ddsReadCheck(bin, type, buffer, count, 0, 0);
         DDS_API_EXIT(n_read);
      }

      /* flush message summary and accumulate read statistics */
      ddsBinMessage(bin, DDS_BIN_MSG_READ);

      if(bin->state & DDS_BIN_FREEZE) {
         /* freeze the history and begin binary i/o */
         ddsBinFreeze(bin);
      }

      if(bin->slice) goto nominal;

      if(bin->media->genus == DDS_MEDIA_NULL) {
         /* nop, media is /dev/null or "memory/buffer" */
         DDS_API_EXIT(0);
      }

      if(bin->state & (DDS_BIN_EOF | DDS_BIN_EOM)) {
         /***********************************/
         /* read another file or more media */
         /***********************************/
         if(bin->state & DDS_BIN_EOM) {
            /* end-of-media detected (double eof) */
            if(ddsOpenNext(bin, "r")) {
               /* no more input media */
               DDS_API_EXIT(0);
            }
         } else if(bin->media->state & DDS_MEDIA_SWAP) {
            /* read another file on current media */
            bin->FileTrc = 1;
            bin->media->file_count++;
         }

         /* offset equals start of PREAMBLE (line header), if any. */
         ggetpos(bin->media->fp, &bin->media->data_bias);

         if((! (bin->state & DDS_BIN_CRAM) || ! bin->media->prev) &&
            type == bin->trace.type) {
            /****************************************/
            /* process format specific line headers */
            /****************************************/
            if (ddsReadPreamble(bin) != DDS_OK) { DDS_API_EXIT(0); }
         }

         /* offset equals start of SEQUENCE (traces). */
         ggetpos(bin->media->fp, &bin->media->trace_bias);
      }
   }

   if(! count || ! type->stroke) {
      /* nothing requested or nothing to transfer */
      DDS_API_EXIT(0);
   }

   /***************************/
   /* Nominal read processing */
   /***************************/

   nominal:
   if(bin->slice) {
      if(type == bin->trace.type) {
         n_read = ddsReadSliceT(bin, type, buffer, count);
      } else {
         n_read = ddsReadSlice(bin, type, buffer, count);
      }
   } else {
      if (bin->media && bin->media->fp) cache = (int)bin->media->fp->cache_r;
      switch(bin->fmt) {
      case DDS_FMT_GENERIC:
         if(type == bin->trace.type) {
            n_read = ddsReadGenT(bin, type, buffer, count);
         } else {
            n_read = ddsReadGen(bin, type, buffer, count);
            ddsStreamReadSwap(swap, buffer, count);
         }
         break;
      case DDS_FMT_USP:
         if(type == bin->trace.type) {
            n_read = ddsReadUspT(bin, type, buffer, count);
         } else if(type->block == bin->std.usp.line.type->block) {
            n_read = ddsReadUspL(bin, type, buffer, count);
         } else {
            n_read = ddsReadGen(bin, type, buffer, count);
            ddsStreamReadSwap(swap, buffer, count);
        }
         break;
      case DDS_FMT_SEGY:
      case DDS_FMT_SEGY1:
         if(type == bin->trace.type) {
            n_read = ddsReadSegyT(bin, type, buffer, count);
         } else if(type->block == bin->std.segy.card.type->block) {
            n_read = ddsReadSegyC(bin, type, buffer, count);
         } else if(type->block == bin->std.segy.binary.type->block) {
            n_read = ddsReadSegyB(bin, type, buffer, count);
         } else {
            n_read = ddsReadGen(bin, type, buffer, count);
            ddsStreamReadSwap(swap, buffer, count);
         }
         break;
      case DDS_FMT_SU:
         if(type == bin->trace.type) {
            n_read = ddsReadSegyT(bin, type, buffer, count);
         } else {
            n_read = ddsReadGen(bin, type, buffer, count);
            ddsStreamReadSwap(swap, buffer, count);
         }
         break;
      }
      if (bin->media && bin->media->fp && (int)bin->media->fp->cache_r != cache) {
         ddsVerbose("cdds_read: %s cache size changed from %d to %d (fd=%d)\n",
            bin->alias_name, cache, (int)bin->media->fp->cache_r, bin->media->fp->fd);
      } 
   }

   DDS_API_EXIT(n_read);
}
