static char rcsid[]="$Id: c_fflush.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, flush binary stream(s)

*/

/***************************************/
/***   Copyright (C) 2003            ***/
/***   BP Corporation, Houston, TX   ***/
/***   Jerry Ehlers, 13 Jan 2003     ***/
/***************************************/

#include "dds_sys.h"

int cdds_fflush(
   BIN_TAG bin_tag)
{
   int n, i, rc = 0;
   DDS_BIN *bin, *slice_bin;
   DDS_MEDIA *media;

   DDS_API_ENTRY("fflush")

   /* if bin_tag < 0, flush all output streams */
   if (bin_tag < 0) {
      rc = gflush((GFILE*)0);
   } else {
      BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));
      if (bin->bin_tag >= 0) {
         bin_tag = bin->bin_tag;
         BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));
      }

      /* if sliced, flush each slice */
      if(bin->slice) {
         for(n=0;n<bin->slice_max;n++) {
            slice_bin = bin->slice[n];
            media = slice_bin->media;
            if (media) {
               i = gflush(media->fp);
               if (!rc) rc = i;
            }
         }
      } else {
         media = bin->media;
         if (media) rc = gflush(media->fp);
      }
   }

   DDS_API_EXIT(rc);
}
