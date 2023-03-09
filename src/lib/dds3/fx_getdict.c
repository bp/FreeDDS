static char rcsid[]="$Id: fx_getdict.c 250 2012-10-09 18:25:44Z user $";
/*
RCS-t=DDS API, convenience routine to get the actual dictionary name

*/

/*****************************************/
/***   Copyright (C) 2003              ***/
/***   BP America, Houston, OK 77079   ***/
/***   Jerry Ehlers, January 2003      ***/
/*****************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fddx_getdict, FDDX_GETDICT)(
   HOST_FTN_STR dict_ftn,
   HOST_FTN_INT *bin_tag,
   HOST_FTN_LEN dict_len
   )
{
   const char *dict;

   DDS_API_INIT;

   dict = cddx_getdict(*bin_tag);

   STR_TO_FTN_STR(dict_ftn, dict_len, dict);

   if (dict == NULL) return DDS_BAD;

   return DDS_OK;
}
