static char rcsid[]="$Id: seekset.c 309 2018-12-18 22:15:38Z user $";
/*
RCS-t=DDS API, seek set within binary data (non-sliced)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_magtape.h"

HOST_INT6_T ddsSeekSet(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   HOST_LLONG_T *TrcOff)
{
   DDS_MEDIA *media;
   HOST_INT6_T new_offset, cur_offset;
   HOST_LLONG_T count;
   HOST_LLONG_T n_read;
   size_t reset;
   long prec = type->prec;
   int n_want, n_chunk, n_bytes;
   char *buf;

   media = bin->media;

   if(offset < 0) {
      ddsError("SeekSet, seek to negative offset\n\t%s, LineTrc= %lu\n",
         media->title.ptr, (unsigned long)bin->LineTrc);
      errno = EINVAL;
      return DDS_BAD;
   }

   new_offset = offset;

   offset *= prec;

   switch(media->genus) {
   case DDS_MEDIA_REG:
      /**********************/
      /* Regular disk file. */
      /**********************/
      cur_offset = offset + media->trace_bias;

      if (bin->state & DDS_BIN_ZIP) {
         ddsError("Error: zip compression currently unsupported\n");
         return DDS_BAD;
      } else if(gsetpos(media->fp, &cur_offset)) {
         ddsError("seek, %s\n\t%s, LineTrc= %lu\n",
            strerror(errno), media->title.ptr,
            (unsigned long)bin->LineTrc);
         new_offset = DDS_BAD;
         break;
      }

      *TrcOff = HOST_INT6_DIV(cur_offset - bin->media->trace_bias,
                              bin->trace.type->prec);
      break;
   case DDS_MEDIA_CHR:
      /*********************/
      /* Character device. */
      /*********************/
      if(offset == 0 && (media->state & DDS_MEDIA_SWAP)) {
         /* Emulate seek to zero offset on tape via rewind. */
         if(! gflush(media->fp)) {
            ddsError("seek flush, %s\n\t%s, LineTrc= %lu\n",
               strerror(errno), media->title.ptr,
               (unsigned long)bin->LineTrc);
         }

         if(! media->prev) {
            /* The first media is already mounted.  Simple rewind. */
#if !defined(_WIN32) && !defined(MINGW)
            if(ddsMTREW(gfileno(media->fp))) {
               ddsError("seek rewind, %s\n\t%s, LineTrc= %lu\n",
                  strerror(errno), media->title.ptr,
                  (unsigned long)bin->LineTrc);
            }
#endif
            media->file_count = 1;
            media->balance = media->length * INCHES_PER_FOOT * media->duty;
            bin->state |= DDS_BIN_EOF;
         } else {
            /* Unload this media and reload the first again. */
            ddsMediaUnload(media);

            if(media->fp) {
               /* stupid kludge */
               gclose(media->fp);
               media->fp = NULL;
            }

            /* Walk back to the first media. */
            while(media->prev) media = media->prev;

            bin->media = media;
            bin->state |= DDS_BIN_EOF;
         }

         *TrcOff = 0;
         break;
      }

      /* No "break" here.  Fall through to FIFO case. */
   case DDS_MEDIA_FIFO:
      /**************************************************/
      /* Character device or pipe media.                */
      /* Emulate forward seek with a sequence of reads. */
      /**************************************************/
      if(ggetpos(media->fp, &cur_offset)) {
         ddsError("seek getpos, %s\n\t%s, LineTrc= %lu\n",
            strerror(errno), media->title.ptr,
            (unsigned long)bin->LineTrc);
         new_offset = DDS_BAD;
         break;
      }

      cur_offset -= media->trace_bias;

      assert(cur_offset >= 0);

      offset -= cur_offset;

      if(offset < 0) {
         ddsError("seek, backward on pipe or tape media\n\t%s, LineTrc= %lu\n",
            media->title.ptr, (unsigned long)bin->LineTrc);
         errno = EINVAL;
         new_offset = DDS_BAD;
         break;
      } else if(HOST_INT6_MOD(cur_offset, prec)) {
         /* Problem: the relative offset between the current and new */
         /* position is NOT a multiple of this type. */
         /* Transform the seek to a byte oriented type. */
         ddsVerbose("seek not aligned,\n\t%s, LineTrc= %lu\n",
            media->title.ptr, (unsigned long)bin->LineTrc);
         new_offset = DDS_BAD;
         type = (DDS_TYPE*)((DDS_SYM_TAG*)(bin->sym_tag.ptr))
            [DDS_STYPE_UNSIGNED1].sym;
         prec = type->prec;
         count = offset;
      } else {
         /* Current offset is a multiple of this type. */
         count = HOST_INT6_DIV(offset, prec);
      }

      *TrcOff = bin->LineTrc - 1;

      if(count > 0) {
         /* Read forward to emulate seek. */
         n_want = 1 + DDS_SEEK_CHUNK / prec;
         reset = dds_tmp.top;
         buf = ddsPushTmp(((count < n_want) ? count : n_want) * prec);

         if (bin->state & DDS_BIN_ZIP) {
            ddsError("Error: zip compression currently unsupported\n");
            return DDS_BAD;
         } else for(n_read = 0; n_read < count; n_read += n_chunk) {
            if(n_want > count - n_read) n_want = count - n_read;

            n_chunk = cdds_read(bin->tag, type->tag, buf, n_want);

            if(! n_chunk) {
               /* This is not necessarily an error if seeking to the end or beyond */
	       /*
		ddsError("seek forward, %s\n\t%s, LineTrc= %lu\n",
                  strerror(errno), media->title.ptr,
                  (unsigned long)bin->LineTrc);
	       */
               if(new_offset > 0) {
                  new_offset -= count - n_read;
               }
               break;
            }
         }

         ddsStackSet(&dds_tmp, reset);

         *TrcOff += HOST_INT6_DIV(n_read * prec, bin->trace.type->prec);
      }

      break;
   default:
      ddsError("seek not allowed\n\t%s, LineTrc= %lu\n",
         media->title.ptr, (unsigned long)bin->LineTrc);
      new_offset = DDS_BAD;
      break;
   }

   return new_offset;
}
