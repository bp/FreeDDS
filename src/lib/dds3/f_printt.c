static char rcsid[]="$Id: f_printt.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, append a definition and next token

FEATURE: Trailing blanks are normally stripped from the value.
A '\0' may be appended to trailing blanks to preserve them.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define VALUE_NOM_LEN 2 * DDS_KB


HOST_FTN_INT
HOST_FTN_FUNC(fdds_printt, FDDS_PRINTT)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_STR value_ftn,
   HOST_FTN_LEN alias_len,
   HOST_FTN_LEN value_len)
{
   static DDS_STRING alias_str = NULL_STRING;
   static DDS_STRING value_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   FTN_STR_TO_STR(value_str, value_ftn, value_len);

   return cdds_printt(alias_str.ptr, value_str.ptr);
}
