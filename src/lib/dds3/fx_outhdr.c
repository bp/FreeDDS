static char rcsid[]="$Id: fx_outhdr.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, convenience routine to open output data with datatype 
                 and specified headers
*/

/***************************************/
/***   Copyright (C) 2006            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, August, 2006    ***/
/***************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fddx_outhdr, FDDX_OUTHDR)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_STR value_ftn,
   HOST_FTN_STR title_ftn,
   HOST_FTN_INT *old_tag,
   HOST_FTN_INT *sprime,
   HOST_FTN_INT *hprime,
   HOST_FTN_STR hdrs_ftn,
   HOST_FTN_LEN alias_len,
   HOST_FTN_LEN value_len,
   HOST_FTN_LEN title_len,
   HOST_FTN_LEN hdrs_len
   )
{
   static DDS_STRING alias_str = NULL_STRING;
   static DDS_STRING value_str = NULL_STRING;
   static DDS_STRING title_str = NULL_STRING;
   static DDS_STRING hdrs_str  = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   FTN_STR_TO_STR(value_str, value_ftn, value_len);

   FTN_STR_TO_STR(title_str, title_ftn, title_len);

   FTN_STR_TO_STR(hdrs_str, hdrs_ftn, hdrs_len);

   return cddx_outhdr(alias_str.ptr, value_str.ptr, title_str.ptr,
                      *old_tag, *sprime, *hprime, hdrs_str.ptr);
}
