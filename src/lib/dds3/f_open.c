static char rcsid[]="$Id: f_open.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, open binary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_open, FDDS_OPEN)(
   HOST_FTN_STR dict_ftn,
   HOST_FTN_STR format_ftn,
   HOST_FTN_STR data_ftn,
   HOST_FTN_STR mode_ftn,
   HOST_FTN_LEN dict_len,
   HOST_FTN_LEN format_len,
   HOST_FTN_LEN data_len,
   HOST_FTN_LEN mode_len)
{
   static DDS_STRING dict = NULL_STRING;
   static DDS_STRING format = NULL_STRING;
   static DDS_STRING data = NULL_STRING;
   static DDS_STRING mode = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(dict, dict_ftn, dict_len);
   FTN_STR_TO_STR(format, format_ftn, format_len);
   FTN_STR_TO_STR(data, data_ftn, data_len);
   FTN_STR_TO_STR(mode, mode_ftn, mode_len);

   return cdds_open(dict.ptr, format.ptr, data.ptr, mode.ptr);
}
