static char rcsid[]="$Id: f_datadefn.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, create filename for binary output, using dictionary rules

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_datadefn, FDDS_DATADEFN)(
   HOST_FTN_STR data_ftn,
   HOST_FTN_STR dict_ftn,
   HOST_FTN_STR alias_ftn,
   HOST_FTN_LEN data_len,
   HOST_FTN_LEN dict_len,
   HOST_FTN_LEN alias_len)
{
   static DDS_STRING dict = NULL_STRING;
   static DDS_STRING alias = NULL_STRING;
   const char *value;

   DDS_API_INIT;

   FTN_STR_TO_STR(dict, dict_ftn, dict_len);

   FTN_STR_TO_STR(alias, alias_ftn, alias_len);

   value = cdds_datadefn(dict.ptr, alias.ptr);

   STR_TO_FTN_STR(data_ftn, data_len, value);

   return DDS_OK;
}
