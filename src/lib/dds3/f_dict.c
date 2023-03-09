static char rcsid[]="$Id: f_dict.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, select (scan/print/suspend/reset/close) dictionary(s).

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define NAME_NOM_LEN 3 * PATHNAME_MAX
#define MODE_NOM_LEN 16


HOST_FTN_INT
HOST_FTN_FUNC(fdds_dict, FDDS_DICT)(
   HOST_FTN_STR name_ftn,
   HOST_FTN_STR mode_ftn,
   HOST_FTN_LEN name_len,
   HOST_FTN_LEN mode_len)
{
   static DDS_STRING name = NULL_STRING;
   static DDS_STRING mode = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(name, name_ftn, name_len);

   FTN_STR_TO_STR(mode, mode_ftn, mode_len);

   return cdds_dict(name.ptr, mode.ptr);
}
