static char rcsid[]="$Id: f_putcb.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, put characters

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_putcb, FDDS_PUTCB)(
   HOST_FTN_INT *bin,
   HOST_FTN_INT *field,
   HOST_FTN_PTR  buf,
   HOST_FTN_INT *offset,
   HOST_FTN_STR array_ftn,
   HOST_FTN_LEN array_len)
{

   DDS_API_INIT;

   return cdds_putcb(*bin, *field, (char*)buf + *offset, array_ftn, array_len);
}
