static char rcsid[]="$Id: c_isreg.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, is this a regular file

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 05 AUG 1996            ***/
/**********************************************/

#include "dds_sys.h"

int cdds_isreg(
   BIN_TAG bin_tag
   )
{
   int i;
   DDS_BIN *bin;
   STRUCTSTAT stat_buf;

   DDS_API_ENTRY("isreg")

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));
   if (bin->bin_tag >= 0) {
      bin_tag = bin->bin_tag;
      BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));
   }

   if (bin->slice_max > 1) {
      /* Check each slice */
      for (i=0;i<bin->slice_max;i++) {
         if (!cdds_isreg(bin->slice[i]->tag)) { DDS_API_EXIT(DDS_FALSE); }
      }
      DDS_API_EXIT(DDS_TRUE);     
   } else {
      if (!  bin->media->fp) { DDS_API_EXIT(DDS_FALSE); }
      
      if (ddsFSTAT(gfileno(bin->media->fp), &stat_buf) < 0) {
         DDS_API_EXIT(DDS_FALSE);
      }

      DDS_API_EXIT(S_ISREG(stat_buf.st_mode));
   }
}
