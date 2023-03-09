static char rcsid[]="$Id: seekend.c 309 2018-12-18 22:15:38Z user $";
/*
RCS-t=DDS API, seek from end within binary data (non-sliced)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_INT6_T ddsSeekEnd(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   HOST_LLONG_T *TrcOff)
{
   DDS_MEDIA *media;
   HOST_INT6_T new_offset;

   media = bin->media;

   switch(media->genus) {
   case DDS_MEDIA_REG:
      /**********************/
      /* Regular disk file. */
      /**********************/
      if (bin->state & DDS_BIN_ZIP) {
         ddsError("Error: zip compression currently unsupported\n");
         return DDS_BAD;
      } else {
         if(gseek(media->fp, 0, SEEK_END)) {
            ddsError("seek end, %s\n\t%s, LineTrc= %lu\n",
                     strerror(errno), media->title.ptr,
                     (unsigned long)bin->LineTrc);
            return DDS_BAD;
         }
         new_offset = ddsSeekCur(bin, type, offset, TrcOff);
      }
      break;
   case DDS_MEDIA_CHR:
   case DDS_MEDIA_FIFO:
      ddsError("seek from END on pipe or tape media\n\t%s, LineTrc= %lu\n",
         media->title.ptr, (unsigned long)bin->LineTrc);
      new_offset = DDS_BAD;
      break;
   default:
      ddsError("seek not allowed\n\t%s, LineTrc= %lu\n",
         media->title.ptr, (unsigned long)bin->LineTrc);
      new_offset = DDS_BAD;
      break;
   }

   return new_offset;
}
