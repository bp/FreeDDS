static char rcsid[]="$Id: seekcur.c 309 2018-12-18 22:15:38Z user $";
/*
RCS-t=DDS API, seek from current within binary data (non-sliced)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_INT6_T ddsSeekCur(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   HOST_LLONG_T *TrcOff)
{
   DDS_MEDIA *media;
   HOST_INT6_T new_offset, cur_offset, trc_offset;
   long prec = type->prec;

   media = bin->media;

   /* Get the current position (byte offset on this media). */
   if (bin->state & DDS_BIN_ZIP) {
      ddsError("Error: zip compression currently unsupported\n");
      return DDS_BAD;
   } else {
      if(ggetpos(media->fp, &cur_offset)) {
         ddsError("seek getpos, %s\n\t%s, LineTrc= %lu\n",
                  strerror(errno), media->title.ptr,
                  (unsigned long)bin->LineTrc);
         return DDS_BAD;
      }
   }

   cur_offset -= media->trace_bias;

   assert(cur_offset >= 0);

   offset *= prec;
   cur_offset += offset;

   if(cur_offset < 0) {
      ddsWarn("SeekCur, seek to negative offset\n\t%s, LineTrc= %lu\n",
         media->title.ptr, (unsigned long)bin->LineTrc);
      cur_offset = 0;
   }

   trc_offset = HOST_INT6_DIV(cur_offset, bin->trace.type->prec);

   new_offset = HOST_INT6_DIV(cur_offset, prec);

   if(HOST_INT6_MOD(cur_offset, prec) != 0) {
      ddsWarn("seek not aligned,\n\t%s, LineTrc= %lu\ncur_offset=%lu prec=%lu bias=%lu offset=%lu\ntrc_offset=%lu new_offset=%lu\n",
         media->title.ptr, (unsigned long)bin->LineTrc,
         (unsigned long)cur_offset, (unsigned long)prec, (unsigned long)(media->trace_bias),
         (unsigned long)offset, (unsigned long)trc_offset, (unsigned long)new_offset);
   }

   if(offset == 0) {
      /* Zero change in the current position. */
      *TrcOff = trc_offset;
      return new_offset;
   }

   switch(media->genus) {
   case DDS_MEDIA_REG:
      /**********************/
      /* Regular disk file. */
      /**********************/
      cur_offset += media->trace_bias;

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
      *TrcOff = trc_offset;
      break;
   case DDS_MEDIA_CHR:
   case DDS_MEDIA_FIFO:
      if(offset < 0) {
         ddsError("seek, backward on pipe or tape media\n\t%s, LineTrc= %lu\n",
            media->title.ptr, (unsigned long)bin->LineTrc);
         errno = EINVAL;
         new_offset = DDS_BAD;
         break;
      }

/***
      cur_offset += media->trace_bias;
***/
      type = (DDS_TYPE*)((DDS_SYM_TAG*)(bin->sym_tag.ptr))
         [DDS_STYPE_UNSIGNED1].sym;
      if(cur_offset != ddsSeekSet(bin, type, cur_offset, TrcOff)) {
         new_offset = DDS_BAD;
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
