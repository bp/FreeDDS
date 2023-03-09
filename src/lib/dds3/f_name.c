static char rcsid[]="$Id: f_name.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, fetch field name from DDS structures

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fdds_name, FDDS_NAME)(
   HOST_FTN_STR name_ftn,
   HOST_FTN_INT *bin,
   HOST_FTN_INT *field,
   HOST_FTN_LEN name_len)
{
   int	i;
   const char *nam;
   static DDS_STRING name = NULL_STRING;

   DDS_API_INIT;

   nam = cdds_name(*bin, *field);
   if (!nam) return DDS_BAD;

   STR_TO_FTN_STR(name_ftn, name_len, nam);

   return DDS_OK;
}
