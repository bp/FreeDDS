static char rcsid[]="$Id: fx_dict.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, convenience routine for dictionary selection

*/

/***************************************/
/***   Copyright (C) 2000            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fddx_dict, FDDX_DICT)(
   HOST_FTN_INT *bin_tag,
   HOST_FTN_STR mode_ftn,
   HOST_FTN_LEN mode_len
   )
{
   static DDS_STRING mode_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(mode_str, mode_ftn, mode_len);

   return cddx_dict(*bin_tag, mode_str.ptr);
}
