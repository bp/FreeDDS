static char rcsid[]="$Id: f_put.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, put bytes

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_put, FDDS_PUT)(
   HOST_FTN_INT *bin,
   HOST_FTN_INT *field,
   HOST_FTN_PTR  buf,
   HOST_FTN_INT *offset,
   HOST_FTN_PTR  array,
   HOST_FTN_INT *count)
{
   DDS_API_INIT;

   return cdds_put(*bin, *field, (char*)buf + *offset, array, *count);
}
