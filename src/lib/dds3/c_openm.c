static char rcsid[]="$Id: c_openm.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, open mapping to output binary data from input

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_openm(
   BIN_TAG out_bin_tag,
   SYM_TAG out_sym_tag,
   BIN_TAG in_bin_tag,
   SYM_TAG in_sym_tag)
{
   DDS_BIN *out_bin, *in_bin;
   DDS_TYPE *out_type, *in_type;
   int status = DDS_OK;

   DDS_API_ENTRY("open map")

   BIN_TAG_TO_BIN(out_bin, out_bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_TYPE(out_type, out_bin, out_sym_tag, DDS_API_EXIT(DDS_BAD));

   BIN_TAG_TO_BIN(in_bin, in_bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_TYPE(in_type, in_bin, in_sym_tag, DDS_API_EXIT(DDS_BAD));

   if(! ddsOpenMap(out_bin, out_type, in_bin, in_type)) {
      status = DDS_BAD;
   }

   DDS_API_EXIT(status)
}
