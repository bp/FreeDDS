static char rcsid[]="$Id: cx_getdict.c 85 2009-06-09 18:18:27Z user $";
/*
RCS-t=DDS API, convenience routine to fetch the actual dictionary name.

*/

/***************************************/
/***   Copyright (C) 2003            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, January, 2003   ***/
/***************************************/

#include "dds_sys.h"

const char * (cddx_getdict)(
   BIN_TAG bin_tag)
{
   DDS_BIN *bin;

   DDS_API_ENTRY("xgetdict")
  
   if (bin_tag < 0 || bin_tag >= dds_bin_max) {
      DDS_API_EXIT(NULL)
   }

   bin = dds_bin[bin_tag];
   if (!bin) {
      DDS_API_EXIT(NULL)
   }

   /* Use output bin if opened & output bin, else use buf bin */

   if (!(bin->state & DDS_BIN_RESERVED) && bin->bin_tag >= 0) {
      bin_tag = bin->bin_tag;
      bin = dds_bin[bin_tag];
      if (!bin) {
         DDS_API_EXIT(NULL)
      }
   }

   DDS_API_EXIT(bin->dict_open)
}
