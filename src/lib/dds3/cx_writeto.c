static char rcsid[]="$Id: cx_writeto.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, convenience write binary data

*/

/***************************************/
/***   Copyright (C) 2000            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"

int cddx_writeto(
   BIN_TAG buf_tag,
   long offset,
   const void *buffer,
   int count)
{
   int n_write;
   long ofst;
   BIN_TAG bin_tag;
   DDS_BIN *bin, *out_bin;

   DDS_API_ENTRY("xwriteto")

   /*****************************/
   /* validate input parameters */
   /*****************************/

   BIN_TAG_TO_BIN(bin, buf_tag, DDS_API_EXIT(0));
   bin_tag = bin->bin_tag;
   BIN_TAG_TO_BIN(out_bin, bin_tag, DDS_API_EXIT(0));

   ofst = cdds_lseek(bin_tag, 0, offset, SEEK_SET);
   if (ofst < 0) {
      cdds_prterr("Seeking output trace %d on %s!\n",
		  offset, out_bin->alias_name);
      DDS_API_EXIT(ofst)
   }

   n_write = cdds_writem(bin_tag, 0, buf_tag, 0, buffer, count);
   if (n_write != count) {
      cdds_prterr("Only wrote %d of %d traces to %s!\n",
		  n_write, count, out_bin->alias_name);
   }

   DDS_API_EXIT(n_write)
}
