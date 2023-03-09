static char rcsid[]="$Id: cx_getbin.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, convenience function to get the associated i/o binary tag

*/

/**********************************************/
/***   Copyright (C) 2001                   ***/
/***   BP Corporation, Houston, TX          ***/
/***   Jerry Ehlers, 13 March 2001          ***/
/**********************************************/

#include "dds_sys.h"

BIN_TAG cddx_getbin(
   BIN_TAG bin_tag)
{
   DDS_BIN *bin;

   DDS_API_ENTRY("xgetbin")

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));

   DDS_API_EXIT(bin->bin_tag);
}
