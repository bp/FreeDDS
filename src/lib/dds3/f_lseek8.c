static char rcsid[]="$Id: f_lseek8.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, logical seek on binary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

#ifdef HOST_INT8_C

HOST_FTN_INT8
HOST_FTN_FUNC(fdds_lseek8, FDDS_LSEEK8)(
   HOST_FTN_INT  *bin_tag,
   HOST_FTN_INT  *sym_tag,
   HOST_FTN_INT8 *offset,
   HOST_FTN_INT  *whence)
{
   DDS_API_INIT;

   return cdds_lseek8(*bin_tag, *sym_tag, *offset, *whence);
}

#endif
