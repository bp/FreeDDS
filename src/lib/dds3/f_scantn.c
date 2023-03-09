static char rcsid[]="$Id: f_scantn.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, select a scan definition and retrieve specific token value.

FEATURE: Trailing blanks are padded as needed to Fortran character values.
The actual length of the defined literal value is returned as an integer.
It may be more or less than the Fortran character string length.

*/

/***********************************/
/***   Copyright (C) 2003        ***/
/***   BP America, Houston, TX   ***/
/***   Jerry Ehlers, Oct 2003    ***/
/***********************************/

#include "dds_sys.h"

#define FMT_NOM_LEN 2 * DDS_KB


HOST_FTN_INT
HOST_FTN_FUNC(fdds_scantn, FDDS_SCANTN)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_STR value_ftn,
   HOST_FTN_INT *count,
   HOST_FTN_LEN alias_len,
   HOST_FTN_LEN value_len)
{
   int status = DDS_OK;
   const char *value;
   static DDS_STRING alias_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   status = cdds_scantn(alias_str.ptr, &value, *count);

   if(status != EOF) STR_TO_FTN_STR(value_ftn, value_len, value);

   return status;
}
