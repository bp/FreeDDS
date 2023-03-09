static char rcsid[]="$Id: f_object.c 39 2009-01-13 23:00:15Z user $";
/*
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 15 April 1997          ***/
/**********************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fdds_object, FDDS_OBJECT)(
   HOST_FTN_INT *bin,
   HOST_FTN_INT *field,
   HOST_FTN_STR name_ftn,
   HOST_FTN_LEN name_len)
{
   static DDS_STRING name = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(name, name_ftn, name_len);

   return cdds_object(*bin, *field, name.ptr);
}
