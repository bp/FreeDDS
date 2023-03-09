static char rcsid[]="$Id: fx_index.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, convenience to return buffer index (zero based) to a
               header

*/

/***************************************/
/***   Copyright (C) 2006            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, August, 2006    ***/
/***************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fddx_index, FDDX_INDEX)(
   HOST_FTN_INT *bin_tag,
   HOST_FTN_STR name_ftn,
   HOST_FTN_INT *prime,
   HOST_FTN_LEN name_len
   )
{
   static DDS_STRING name_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(name_str, name_ftn, name_len);

   return cddx_index(*bin_tag, name_str.ptr, *prime);
}
