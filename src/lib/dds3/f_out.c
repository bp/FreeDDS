static char rcsid[]="$Id: f_out.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, initialize dictionary for binary output data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define TITLE_NOM_LEN 256


HOST_FTN_INT
HOST_FTN_FUNC(fdds_out, FDDS_OUT)(
   HOST_FTN_STR dict_ftn,
   HOST_FTN_STR alias_ftn,
   HOST_FTN_STR value_ftn,
   HOST_FTN_STR history_ftn,
   HOST_FTN_LEN dict_len,
   HOST_FTN_LEN alias_len,
   HOST_FTN_LEN value_len,
   HOST_FTN_LEN history_len)
{
   const char *dict;
   static DDS_STRING alias_str = NULL_STRING;
   static DDS_STRING value_str = NULL_STRING;
   static DDS_STRING history_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   FTN_STR_TO_STR(value_str, value_ftn, value_len);

   FTN_STR_TO_STR(history_str, history_ftn, history_len);

   dict = cdds_out(alias_str.ptr, value_str.ptr, history_str.ptr);

   STR_TO_FTN_STR(dict_ftn, dict_len, dict);

   return DDS_OK;
}
