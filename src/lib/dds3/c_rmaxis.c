static char rcsid[]="$Id: c_rmaxis.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, remove an existing axis from a dictionary.

*/

/***************************************/
/***   Copyright (C) 2001            ***/
/***   BP Corporation, Houston, TX   ***/
/***   Jerry Ehlers, July 2001       ***/
/***************************************/

#include "dds_sys.h"

int cdds_rmaxis(
   const char *dict,
   int num)
{
   int i, naxis;
   char axis[RANK_MAX][AXISNAME_MAX];

   DDS_API_ENTRY("rmaxis")

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
