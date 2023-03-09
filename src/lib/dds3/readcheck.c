static char rcsid[]="$Id: readcheck.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, check results of read binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_magtape.h"

int ddsReadCheck(
   DDS_BIN *bin,
   DDS_TYPE *type,
   void *buffer,
   int count,
   int n_read,
   int residual)
{
   DDS_MEDIA *media;
   int n_more, saved_errno;
   char *buf, *ebuf;

   saved_errno = errno;
   media = bin->media;

   if(residual) {
      /* Fractional type was read. */
      if(! (media->state & DDS_MEDIA_SWAP) ||
         ((bin->state & DDS_BIN_EOM) && ! media->next)) {
         /* This is definitely an error. */
         /* 18 Feb 1998 RLS, WRONG... only a warning. */
         /*    it could be a disk EOF (green==0) */
         ddsWarn("residual read bytes= %i\n\t%s, ReelTrc= %lu\n",
            residual,
            media->title.ptr, (unsigned long)bin->ReelTrc);
      } else if(bin->fmt == DDS_FMT_USP) {
         if(! (bin->state & DDS_BIN_CRAM)) {
            ddsError("residual usp read bytes= %i\n\t%s, ReelTrc= %lu\n",
               residual,
               media->title.ptr, (unsigned long)bin->ReelTrc);
         }
      } else if(bin->fmt == DDS_FMT_SEGY || bin->fmt == DDS_FMT_SEGY1) {
         if(bin->state & DDS_BIN_UNBLOCK) {
            if(! (bin->std.segy.flags & DDS_SEGY_charisma) ||
               residual < bin->std.segy.card_green_min ||
               residual > bin->std.segy.card_green_max) {
               ddsError("residual segy read bytes= %i\n\t%s, ReelTrc= %lu\n",
                  residual,
                  media->title.ptr, (unsigned long)bin->ReelTrc);
            }
         }
      } else if(bin->fmt == DDS_FMT_SU) {
         if(bin->state & DDS_BIN_UNBLOCK) {
            ddsError("residual segy read bytes= %i\n\t%s, ReelTrc= %lu\n",
               residual,
               media->title.ptr, (unsigned long)bin->ReelTrc);
         }
      } else if(bin->state & DDS_BIN_UNBLOCK) {
         ddsWarn("residual read bytes= %i\n\t%s, ReelTrc= %lu\n",
            residual,
            media->title.ptr, (unsigned long)bin->ReelTrc);
      }
   }

   if(saved_errno) {
      /* Diagnose the error that was detected. */
      if(media->state & DDS_MEDIA_SWAP) {
#if !defined(_WIN32) && !defined(MINGW)
         ddsMTStatus(gfileno(media->fp), "read media");
#endif
      }

      ddsError("read: %s\n\t%s, ReelTrc= %lu\n\ttype= %s\n",
         strerror(saved_errno),
         media->title.ptr, (unsigned long)bin->ReelTrc,
         ddsHintType(bin, type));
   }

   if(media->state & DDS_MEDIA_SWAP) {
      /* swap media processing */
      if(n_read > 0) {
         /* some data (non-eof) has been read */
         bin->state &= (DDS_BIN_STATE) ~ (DDS_BIN_EOF | DDS_BIN_EOM);

         media->balance -= n_read * (media->gap + media->toll * type->stroke);

         /* work around for 2 gigabyte file limit on ExaByte tape */
         if(0X40000000 < (media->ExaByteFix += n_read * type->stroke)) {
            /* seek 0 byte offset from beginning */
            gseek(media->fp, (long)0, SEEK_SET);
            media->ExaByteFix = 0;
         }
      }

      if(n_read < count) {
         /* eof detected and more data is wanted */
         if((bin->state & DDS_BIN_EOF) ||
            ! (bin->state & DDS_BIN_FILES)) {
            /* double eof is eom, or one file per media */
            bin->state |= DDS_BIN_EOM;
         } else {
            bin->state |= DDS_BIN_EOF;
         }

         if(! (bin->state & DDS_BIN_PADDING) ||
            type != bin->trace.type) {
            /* read more data, recursively */
            n_more = count - n_read;

            n_read += cdds_read(bin->tag, type->tag,
               (char*)buffer + n_read * type->stroke, n_more);

            /*********************************************************/
            /* Caution: bin->media will change, if media is mounted. */
            /*********************************************************/
         }
      }
   } else if(n_read > 0) {
      /* some data (non-eof) has been read */
      bin->state &= (DDS_BIN_STATE) ~ (DDS_BIN_EOF | DDS_BIN_EOM);
   }

   if(n_read) bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_WRITTEN;

   if(type == bin->trace.type && n_read < count) {
      /* Final padding of dead traces because */
      /* end-of-media has been detected on final media. */
      bin->state |= DDS_BIN_PADDING;
      buf = (char*)buffer + n_read * type->stroke;

      while(n_read < count) {
         ebuf = ddsEditFinal(bin, type, DDS_BIN_MSG_READ);

         if(! ebuf) break;  /* padding done */

         /* pass a dead trace to the API */
         memcpy(buf, ebuf, type->stroke);

         /* Counters reflect the media (not API). */
         bin->LineTrc--;
         bin->ReelTrc--;
         bin->FileTrc--;

         n_read++;
         buf += type->stroke;
         ddsCubeInc(bin);
      }
   }

   return n_read;
}
