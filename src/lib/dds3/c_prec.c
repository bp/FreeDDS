static char rcsid[]="$Id: c_prec.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, return byte precision of type symbol

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 15 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_prec(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
   /* return byte precision (effective) associated with field */
   DDS_BIN *bin;
   DDS_TYPE *type;

   DDS_API_ENTRY("prec");

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_TYPE(type, bin, sym_tag, DDS_API_EXIT(DDS_BAD));

   DDS_API_EXIT(type->stroke);
}
