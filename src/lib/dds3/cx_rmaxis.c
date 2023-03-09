static char rcsid[]="$Id: cx_rmaxis.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, remove an existing axis from a dictionary.

*/

/***************************************/
/***   Copyright (C) 2001            ***/
/***   BP Corporation, Houston, TX   ***/
/***   Jerry Ehlers, July 2001       ***/
/***************************************/

#include "dds_sys.h"

int cddx_rmaxis(
   BIN_TAG bin_tag,
   int num)
{
   int i, naxis;
   const char *dict;
   char axis[RANK_MAX][AXISNAME_MAX];
   DDS_BIN *bin;

   DDS_API_ENTRY("xrmaxis")

   if (bin_tag < 0 || bin_tag >= dds_bin_max) {
      DDS_API_EXIT(DDS_BAD)
   }

   bin = dds_bin[bin_tag];
   if (!bin) {
      DDS_API_EXIT(DDS_BAD)
   }

   dict = bin->dict_open;
   cdds_dict(dict, "scan");

   naxis = cdds_scank("axis", "");
   if (naxis <= 1 || num < 1 || num > naxis) {
     DDS_API_EXIT(DDS_BAD)
   }

   cdds_scanf("axis", "");
   for(i=0;i<naxis;i++) cdds_scanf("", " %s", axis[i]);

   cdds_dict(dict, "print");
   cdds_printf("axis", "");
   for(i=0;i<naxis;i++) {
     if (i+1 != num) cdds_printf("", " %s", axis[i]);
   }
   cdds_printf("", "\n");

   cdds_dict(dict, "scan");
   cdds_dict(dict, "print");

   DDS_API_EXIT(DDS_OK)
}
