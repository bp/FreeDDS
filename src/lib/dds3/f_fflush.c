static char rcsid[]="$Id: f_fflush.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, flush binary stream(s)

*/

/***************************************/
/***   Copyright (C) 2003            ***/
/***   BP Corporation, Houston, TX   ***/
/***   Jerry Ehlers, 13 Jan 2003     ***/
/***************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_fflush, FDDS_FFLUSH)(
   HOST_FTN_INT *bin_tag)
{
   DDS_API_INIT;

   return cdds_fflush(*bin_tag);
}
