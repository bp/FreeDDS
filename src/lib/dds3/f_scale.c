static char rcsid[]="$Id: f_scale.c 67 2009-03-06 23:03:49Z user $";
/*
RCS-t=DDS API, compute scale factor, given output and input units.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define UNIT_NOM_LEN 32


HOST_FTN_DBL 
HOST_FTN_FUNC(fdds_scale, FDDS_SCALE)(
   HOST_FTN_STR out_ftn,
   HOST_FTN_STR in_ftn,
   HOST_FTN_LEN out_len,
   HOST_FTN_LEN in_len)
{
   static DDS_STRING out = NULL_STRING;
   static DDS_STRING in = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(out, out_ftn, out_len);
   FTN_STR_TO_STR(in, in_ftn, in_len);

   return (HOST_FTN_DBL)cdds_scale(out.ptr, in.ptr);
}
