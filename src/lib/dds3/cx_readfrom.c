static char rcsid[]="$Id: cx_readfrom.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, convenience read binary data

*/

/***************************************/
/***   Copyright (C) 2000            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"

int cddx_readfrom(
   BIN_TAG buf_tag,
   long offset,
   void *buffer,
   int count)
{
   int n_read;
   long ofst;
   BIN_TAG bin_tag;
   DDS_BIN *bin, *in_bin;

   DDS_API_ENTRY("xreadfrom")

   /*****************************/
   /* validate input parameters */
   /*****************************/

   BIN_TAG_TO_BIN(bin, buf_tag, DDS_API_EXIT(0));
   bin_tag = bin->bin_tag;
   BIN_TAG_TO_BIN(in_bin, bin_tag, DDS_API_EXIT(0));

   ofst = cdds_lseek(bin_tag, 0, offset, SEEK_SET);
   if (ofst < 0) {
      cdds_prtcon("WARN: Seeking input trace %d on %s!\n",
		  offset, in_bin->alias_name);
      DDS_API_EXIT(ofst)
   }


   n_read = cdds_readm(bin_tag, 0, buf_tag, 0, buffer, count);
   if (n_read < 0) {
      cdds_prterr("Reading from %s!\n", in_bin->alias_name);
   } else if (n_read != count) {
      cdds_prtmsg("End-of-File from %s!\n", in_bin->alias_name);
   }

   DDS_API_EXIT(n_read)
}
