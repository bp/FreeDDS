static char rcsid[]="$Id: c_close.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, close binary data, given a bin_tag

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_close(
   BIN_TAG bin_tag)
{
   BIN_TAG tag;
   DDS_BIN *bin;

   DDS_API_ENTRY("close")

   if (bin_tag < 0 || bin_tag >= dds_bin_max) { DDS_API_EXIT(0); }

   bin = dds_bin[bin_tag];
   if (!bin) { DDS_API_EXIT(0); }

   tag = bin->bin_tag;
   if (tag >= 0 && tag < dds_bin_max) {
      ddsCloseBin(dds_bin[tag]);
   }

   DDS_API_EXIT(ddsCloseBin(bin))
}
