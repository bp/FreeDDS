static char rcsid[]="$Id: f_rmaxis.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, remove an existing axis from a dictionary.

*/

/***************************************/
/***   Copyright (C) 2001            ***/
/***   BP Corporation, Houston, TX   ***/
/***   Jerry Ehlers, July 2001       ***/
/***************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_rmaxis, FDDS_RMAXIS)(
   HOST_FTN_STR dict_ftn,
   HOST_FTN_INT *num,
   HOST_FTN_LEN dict_len)
{
   static DDS_STRING dict = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(dict, dict_ftn, dict_len);

   return cdds_rmaxis(dict.ptr, *num);
}
