static char rcsid[]="$Id: f_binfd.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, binary file descriptor.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_binfd, FDDS_BINFD)(
   HOST_FTN_INT *bin_tag)
{
   DDS_API_INIT;

   return cdds_binfd(*bin_tag);
}
