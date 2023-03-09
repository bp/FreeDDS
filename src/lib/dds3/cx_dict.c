static char rcsid[]="$Id: cx_dict.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, convenience select (scan/print/suspend/reset/close) dictionary(s).

*/

/***************************************/
/***   Copyright (C) 2000            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"

int (cddx_dict)(
   BIN_TAG bin_tag,
   const char *mode)
{
   int status;
   DDS_BIN *bin;

   DDS_API_ENTRY("xdict")
  
   if (bin_tag < 0 || bin_tag >= dds_bin_max) {
      DDS_API_EXIT(EOF)
   }

   bin = dds_bin[bin_tag];
   if (!bin) {
      DDS_API_EXIT(EOF)
   }

   /* Use output bin if opened & output bin, else uas buf bin */

   if (!(bin->state & DDS_BIN_RESERVED) && bin->bin_tag >= 0) {
      bin_tag = bin->bin_tag;
      bin = dds_bin[bin_tag];
      if (!bin) {
         DDS_API_EXIT(EOF)
      }
   }

   status = cdds_dict(bin->dict_open, mode);
   DDS_API_EXIT(status)
}
