static char rcsid[]="$Id: fx_in.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, convenience routine for binary input data with Samples only
*/

/***************************************/
/***   Copyright (C) 2000            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"

#define TITLE_NOM_LEN 256


HOST_FTN_INT
HOST_FTN_FUNC(fddx_in, FDDX_IN)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_STR value_ftn,
   HOST_FTN_STR title_ftn,
   HOST_FTN_LEN alias_len,
   HOST_FTN_LEN value_len,
   HOST_FTN_LEN title_len
   )
{
   static DDS_STRING alias_str = NULL_STRING;
   static DDS_STRING value_str = NULL_STRING;
   static DDS_STRING title_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   FTN_STR_TO_STR(value_str, value_ftn, value_len);

   FTN_STR_TO_STR(title_str, title_ftn, title_len);

   return cddx_in(alias_str.ptr, value_str.ptr, title_str.ptr);
}
