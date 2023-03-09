static char rcsid[]="$Id: fx_addaxis.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, add a new axis to a dictionary.

*/

/***************************************/
/***   Copyright (C) 2001            ***/
/***   BP Corporation, Houston, TX   ***/
/***   Jerry Ehlers, July 2001       ***/
/***************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fddx_addaxis, FDDX_ADDAXIS)(
   HOST_FTN_INT *bin_tag,
   HOST_FTN_INT *num,
   HOST_FTN_STR name_ftn,
   HOST_FTN_LEN name_len
   )
{
   static DDS_STRING name = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(name, name_ftn, name_len);

   return cddx_addaxis(*bin_tag, *num, name.ptr);
}
