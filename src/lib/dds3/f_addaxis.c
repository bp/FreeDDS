static char rcsid[]="$Id: f_addaxis.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, add a new axis to a dictionary.

*/

/***************************************/
/***   Copyright (C) 2001            ***/
/***   BP Corporation, Houston, TX   ***/
/***   Jerry Ehlers, July 2001       ***/
/***************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_addaxis, FDDS_ADDAXIS)(
   HOST_FTN_STR dict_ftn,
   HOST_FTN_INT *num,
   HOST_FTN_STR name_ftn,
   HOST_FTN_LEN dict_len,
   HOST_FTN_LEN name_len)
{
   static DDS_STRING dict = NULL_STRING;
   static DDS_STRING name = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(dict, dict_ftn, dict_len);

   FTN_STR_TO_STR(name, name_ftn, name_len);

   return cdds_addaxis(dict.ptr, *num, name.ptr);
}
