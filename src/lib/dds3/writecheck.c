static char rcsid[]="$Id: writecheck.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, check results of write binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_magtape.h"

int ddsWriteCheck(
   DDS_BIN *bin,
   DDS_TYPE *type,
   const void *buffer,
   int count,
   int n_write,
   int residual)
{
   DDS_MEDIA *media;
   int n_more, saved_errno;
   int n_want, n_bytes;
   DDS_TYPE *u1_type;

   saved_errno = errno;
   media = bin->media;

   if(residual) {
      /* Fractional type was written. */
      if(! (media->state & DDS_MEDIA_SWAP)) {
         /* This is definitely an error. */
         ddsError("residual write bytes= %i\n\t%s, ReelTrc= %lu\n",
            residual,
            media->title.ptr, (unsigned long)bin->ReelTrc);
      } else if(bin->fmt == DDS_FMT_USP) {
         if(! (bin->state & DDS_BIN_CRAM)) {
            ddsError("residual usp write bytes= %i\n\t%s\n",
               residual,
               media->title.ptr, (unsigned long)bin->ReelTrc);
         }
      } else if(bin->state & DDS_BIN_UNBLOCK) {
         ddsError("residual write bytes= %i\n\t%s, ReelTrc= %lu\n",
            residual,
            media->title.ptr, (unsigned long)bin->ReelTrc);
      }
   }

   if(saved_errno) {
      /* Diagnose the error that was detected. */
      if(media->state & DDS_MEDIA_SWAP) {
#if !defined(_WIN32) && !defined(MINGW)
         ddsMTStatus(gfileno(media->fp), "write media");
#endif
      }

      ddsError("write: %s\n\t%s, ReelTrc= %lu\n\ttype= %s\n",
         strerror(saved_errno),
         media->title.ptr, (unsigned long)bin->ReelTrc,
         ddsHintType(bin, type));
   }

   if(media->state & DDS_MEDIA_SWAP) {
      /* swap media processing */
      if(n_write > 0) {
         /* some data (non-eof) has been written */
         bin->state &= (DDS_BIN_STATE) ~ (DDS_BIN_EOF | DDS_BIN_EOM);
         /* data (non-eof) has been write */

         media->balance -= n_write * (media->gap + media->toll * type->stroke);

         /* work around for 2 gigabyte file limit on ExaByte tape */
         if(0X40000000 < (media->ExaByteFix += n_write * type->stroke)) {
            /* seek 0 byte offset from beginning */
            gseek(media->fp, (long)0, SEEK_SET);
            media->ExaByteFix = 0;
         }
      }

      if(n_write < count ||  /* physical end-of-media */
         (! bin->cube.axis[bin->cube.cleave].ordinal && /* chunk boundary */
            media->balance < bin->cube.chunk && /* and near end-of-media */
            bin->trace.type  == type)) {  /* and writing trace data */
         /* end-of-media detected or trace at chunk boundary near eom */

         if(! (bin->state & DDS_BIN_EOM)) {
            /* some data has been written (not a double eof) */
            bin->state |= (DDS_BIN_STATE)(DDS_BIN_EOF | DDS_BIN_EOM);

            /* write more data, recursively */
            if(residual) {
               if(bin->state & DDS_BIN_UNBLOCK) {
                  /* fatal error (unblocked write) */
                  n_more = 0;
               } else {
                  /* write residual data from last fragment */
                  n_want = type->stroke - residual;
                  u1_type = (DDS_TYPE*)
                     ((DDS_SYM_TAG*)(bin->sym_tag.ptr))[DDS_STYPE_UNSIGNED1].sym;
/* inhibit data dump ? */
                  n_bytes = cdds_write(bin->tag, u1_type->tag,
                     (char*)buffer + n_write * type->stroke + residual, n_want);

                  if(n_bytes == n_want) {
                     /* residual write successful */
/*    dump data ? */
                     n_write++;
                     n_more = count - n_write;
                  } else {
                     /* fatal error doing residual write */
                     n_more = 0;
                  }
               }
            } else {
               n_more = count - n_write;

               if(! n_more && ! (bin->state & DDS_BIN_UNBLOCK)) {
                  /* Flush the buffer contents, before changing media. */
                  /* This changes the media on a record boundary, */
                  /* unlike usp's cram which fragments a trace. */
                  gflush(media->fp);
               }
            }

            if(n_more) {
               n_write += cdds_write(bin->tag, type->tag,
                  (char*)buffer + n_write * type->stroke, n_more);
            }

            /*********************************************************/
            /* Caution: bin->media will change, if media is mounted. */
            /*********************************************************/
         }
      }
   } else if(n_write == count) {
      /* all requested data has been written */
      bin->state &= (DDS_BIN_STATE) ~ (DDS_BIN_EOF | DDS_BIN_EOM);
   } else {
      /* some data has not been written */
      assert(saved_errno);
      bin->state |= (DDS_BIN_STATE)(DDS_BIN_EOF | DDS_BIN_EOM);
   }

   if(n_write && type == bin->trace.type) bin->state |= DDS_BIN_WRITTEN;

   return n_write;
}
