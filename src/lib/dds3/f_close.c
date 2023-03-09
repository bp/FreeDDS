static char rcsid[]="$Id: f_close.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, close binary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_close, FDDS_CLOSE)(
   HOST_FTN_INT *bin_tag)
{
   DDS_API_INIT;

   return cdds_close(*bin_tag);
}
