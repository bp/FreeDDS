static char rcsid[]="$Id: c_write.c 229 2011-06-02 18:53:57Z user $";
/*
RCS-t=DDS API, edit and write binary data

*/

/**********************************************/
/***   Copyright (C) 1996,2004              ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_swap.h"
#include "dds_magtape.h"

int cdds_write(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const void *buffer,
   int count)
{
   DDS_BIN *bin, *master_bin, *former_bin;
   DDS_TYPE *type, *detype;
   HOST_INT6_T new_offset;
   int n_write, cache;
   char *line_pad;
   size_t line_eod;
   /* These are for John's special I/O */
   int swap=0; /* for STREAM_TO_SYM_TAG & ddsStreamWriteSwap */
   static const void *outbuf = 0;  /* for ddsStreamWriteSwap */
   static int buflen = 0;          /* for ddsStreamWriteSwap */

   DDS_API_ENTRY("write")

   /*****************************/
   /* validate input parameters */
   /*****************************/

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(0));

   STREAM_TO_SYM_TAG(sym_tag, swap);

   SYM_TAG_TO_TYPE(type, bin, sym_tag, DDS_API_EXIT(0));

   NON_NEGATIVE(count, bin, DDS_API_EXIT(0));

   NON_NULL(buffer, bin, DDS_API_EXIT(0));

   /********************************/
   /* Keep tabs on Samples written */
   /********************************/

   dds_samples_out += count *  bin->cube.axis[1].size;

   if(bin->state & (DDS_BIN_FREEZE | DDS_TRACE_READ |
      DDS_BIN_DESEEK | DDS_BIN_EOF | DDS_BIN_EOM |
      DDS_BIN_MSG_READ | DDS_BIN_MSG_SEEK)) {
      /*****************************/
      /* write exception processing */
      /*****************************/

      /*********************************************************/
      /* Caution: bin->media will change, if media is mounted. */
      /*********************************************************/

      if(bin->state & DDS_TRACE_READ) {
         ddsPanic("write with DDS_TRACE_READ set\n");
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

      /* flush message summary and accumulate write statistics */ 
      ddsBinMessage(bin, DDS_BIN_MSG_WRITE);

      if(bin->state & DDS_BIN_FREEZE) {
         /* freeze the history and begin binary i/o */
         ddsBinFreeze(bin);

         if(bin->data_dict && (bin->state & DDS_BIN_OPEN_CREATE)) {
            /*********************************************************/
            /* Binary data is attached to an output dictionary.      */
            /*                                                       */
            /* Append special defn to delimit dictionary and binary. */
            /* i.e. append "DDS:END_OF_DICT=...#",                   */
            /*    where "..." are blanks and                         */
            /*    "#" is the DDS_EOD_CHAR character '\004'.          */
            /*    The actual number of blanks is adjusted to force   */
            /*    the binary data to start on an efficient offset.   */
            /*********************************************************/
            line_pad = (char*)ddsMalloc(DDS_EOD_MODULO + 3 +
               strlen(EOD_DEFN_NAME));

            sprintf(line_pad, "%s=%*s",
               EOD_DEFN_NAME, (int)DDS_EOD_MODULO, "");

            line_eod = strlen(EOD_DEFN_NAME) + 2 + DDS_EOD_MODULO -
               (bin->data_dict->buf_flush + strlen(EOD_DEFN_NAME) + 2) %
               DDS_EOD_MODULO;

            line_pad[line_eod - 1] = DDS_EOD_CHAR;

            gwritec(bin->media->fp, line_pad, line_eod);

            ddsFree(line_pad);
         }
      }

      if(bin->slice) goto nominal;

      if(bin->media->genus == DDS_MEDIA_NULL) {
         /* nop, media is /dev/null or "memory/buffer" */
         DDS_API_EXIT(0);
      }

      if(bin->state & (DDS_BIN_EOF | DDS_BIN_EOM)) {
         /************************************/
         /* write another file or more media */
         /************************************/
         if(bin->state & DDS_BIN_EOM) {
            if(bin->state & DDS_BIN_EOF) {
               /* Real end-of-media detected (forced media load). */
               if(ddsOpenNext(bin, "w")) {
                  /* no more output media */
                  DDS_API_EXIT(0);
               }
            } else {
               /* Fake end-of-media, write eof and retain this media. */
               if(gflush(bin->media->fp)) {
                  ddsWarn("%s\n\tbinary= %s\n", strerror(errno), bin->name);
                  /* oh bad luck, end-of-media while flushing buffer ? */
                  if(ddsOpenNext(bin, "w")) {
                     /* no more output media */
                     bin->state |= DDS_BIN_EOM;
                     DDS_API_EXIT(0);
                  }
               }
#if !defined(_WIN32) && !defined(MINGW)
               if(! ddsMTWEOF(gfileno(bin->media->fp), 1)) {
                  /* write another file on current media */
                  bin->media->file_count++;
                  bin->state |= DDS_BIN_EOF;
                  bin->FileTrc = 1;
               } else {
                  /* oh bad luck, EOM while writing EOF ? */
                  if(ddsOpenNext(bin, "w")) {
                     /* no more output media */
                     bin->state |= DDS_BIN_EOM;
                     DDS_API_EXIT(0);
                  }
               }
#endif
            }
         }

         /* offset equals start of PREAMBLE (line header). */
         ggetpos(bin->media->fp, &bin->media->data_bias);

         if((! (bin->state & DDS_BIN_CRAM) || ! bin->media->prev) &&
            (type == bin->trace.type || type->tag == DDS_CHAR)) {
            /****************************************/
            /* process format specific line headers */
            /****************************************/
            if (ddsWritePreamble(bin) != DDS_OK) { DDS_API_EXIT(0); }
         }

         /* offset equals start of SEQUENCE (traces). */
         ggetpos(bin->media->fp, &bin->media->trace_bias);
         bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_EOF;
      }
   }

   if(! count || ! type->stroke) {
      /* nothing requested or nothing to transfer */
      DDS_API_EXIT(0);
   }

   /***************************/
   /* Nominal write processing */
   /***************************/

   nominal:
   if(bin->slice) {
      if(type == bin->trace.type) {
         n_write = ddsWriteSliceT(bin, type, buffer, count);
      } else {
         n_write = ddsWriteSlice(bin, type, buffer, count);
      }
   } else {
      if (bin->media && bin->media->fp) cache = (int)bin->media->fp->cache_w;
      switch(bin->fmt) {
      case DDS_FMT_GENERIC:
         if(type == bin->trace.type) {
            n_write = ddsWriteGenT(bin, type, buffer, count);
         } else {
            ddsStreamWriteSwap(swap, buffer, buflen, outbuf, count);
            n_write = ddsWriteGen(bin, type, outbuf, count);
         }
         break;
      case DDS_FMT_USP:
         if(type == bin->trace.type) {
            n_write = ddsWriteUspT(bin, type, buffer, count);
         } else if(type->block == bin->std.usp.line.type->block) {
            n_write = ddsWriteUspL(bin, type, buffer, count);
         } else {
            ddsStreamWriteSwap(swap, buffer, buflen, outbuf, count);
            n_write = ddsWriteGen(bin, type, outbuf, count);
         }
         break;
      case DDS_FMT_SEGY:
      case DDS_FMT_SEGY1:
         if(type == bin->trace.type) {
            n_write = ddsWriteSegyT(bin, type, buffer, count);
         } else if(type->block == bin->std.segy.card.type->block) {
            n_write = ddsWriteSegyC(bin, type, buffer, count);
         } else if(type->block == bin->std.segy.binary.type->block) {
            n_write = ddsWriteSegyB(bin, type, buffer, count);
         } else {
            ddsStreamWriteSwap(swap, buffer, buflen, outbuf, count);
            n_write = ddsWriteGen(bin, type, outbuf, count);
         }
         break;
      case DDS_FMT_SU:
         if(type == bin->trace.type) {
            n_write = ddsWriteSegyT(bin, type, buffer, count);
         } else {
            ddsStreamWriteSwap(swap, buffer, buflen, outbuf, count);
            n_write = ddsWriteGen(bin, type, outbuf, count);
         }
         break;
      }
      if (bin->media && bin->media->fp && (int)bin->media->fp->cache_w != cache) {
         ddsVerbose("cdds_write: %s cache size changed from %d to %d (fd=%d)\n",
            bin->alias_name, cache, (int)bin->media->fp->cache_w, bin->media->fp->fd);
      } 
   }

   DDS_API_EXIT(n_write);
}
