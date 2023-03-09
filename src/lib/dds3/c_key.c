static char rcsid[]="$Id: c_key.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, search entire value of scan definition for keywords

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_key(
   const char *keys)
{
   int status = DDS_OK;

   DDS_API_ENTRY("key")

   if(dds_scan_dict) {
      /* always start from the beginning of the value */
      dds_scan_token.next = dds_scan_dict->buf.ptr + dds_scan_value_bias;
      if(keys) while(isspace(*keys)) keys++;
      status = ddsKey(keys);
      if(keys && *keys && ! status) status = EOF; /* key not found */
   } else {
      status = EOF;
   }
   DDS_API_EXIT(status);
}
