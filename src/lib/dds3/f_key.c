static char rcsid[]="$Id: f_key.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, search entire value of scan definition for keywords

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define KEYS_NOM_LEN 32 * DEFNNAME_MAX


HOST_FTN_INT
HOST_FTN_FUNC(fdds_key, FDDS_KEY)(
   HOST_FTN_STR keys_ftn,
   HOST_FTN_LEN keys_len)
{
   static DDS_STRING keys = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(keys, keys_ftn, keys_len);

   return cdds_key(keys.ptr);
}
