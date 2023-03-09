static char rcsid[]="$Id: f_write.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, write to binary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_write, FDDS_WRITE)(
   HOST_FTN_INT *bin_tag,
   HOST_FTN_INT *sym_tag,
   HOST_FTN_PTR  buf,
   HOST_FTN_INT *count)
{
   DDS_API_INIT;

   return cdds_write(*bin_tag, *sym_tag, buf, *count);
}
