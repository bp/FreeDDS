static char rcsid[]="$Id: c_align.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, return byte alignment required by type symbol

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 15 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_align(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
   DDS_BIN *bin;
   DDS_TYPE *type;

   DDS_API_ENTRY("align");

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_TYPE(type, bin, sym_tag, DDS_API_EXIT(DDS_BAD));

   DDS_API_EXIT(type->align);
}
