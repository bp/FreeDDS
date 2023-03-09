static char rcsid[]="$Id: f_puti2.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, put integer*2

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_puti2, FDDS_PUTI2)(
   HOST_FTN_INT *bin_tag,
   HOST_FTN_INT *sym_tag,
   HOST_FTN_PTR  buf,
   HOST_FTN_INT *bias,
   HOST_FTN_INT2 *array,
   HOST_FTN_INT *count)
{
   DDS_API_INIT;

   return cdds_puts(*bin_tag, *sym_tag, (char*)buf + *bias, array, *count);
}
