static char rcsid[]="$Id: f_scank.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, select a scan definition and search for keywords.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define KEYS_NOM_LEN 2 * DDS_KB


HOST_FTN_INT
HOST_FTN_FUNC(fdds_scank, FDDS_SCANK)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_STR keys_ftn,
   HOST_FTN_LEN alias_len,
   HOST_FTN_LEN keys_len)
{
   static DDS_STRING alias_str = NULL_STRING;
   static DDS_STRING keys_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   FTN_STR_TO_STR(keys_str, keys_ftn, keys_len);

   return cdds_scank(alias_str.ptr, keys_str.ptr);
}
