static char rcsid[]="$Id: f_newer.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, find newer definition, given alias list.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 20 Aug 1996            ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_newer, FDDS_NEWER)(
   HOST_FTN_STR name_ftn,
   HOST_FTN_STR alias_ftn,
   HOST_FTN_LEN name_len,
   HOST_FTN_LEN alias_len)
{
   static DDS_STRING alias_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   STR_TO_FTN_STR(name_ftn, name_len, cdds_newer(alias_str.ptr));

   return DDS_OK;
}
