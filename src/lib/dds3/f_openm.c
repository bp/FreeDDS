static char rcsid[]="$Id: f_openm.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, open mapping to output binary data from input.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_openm, FDDS_OPENM)(
   HOST_FTN_INT *out_bin_tag,
   HOST_FTN_INT *out_sym_tag,
   HOST_FTN_INT *in_bin_tag,
   HOST_FTN_INT *in_sym_tag)
{
   DDS_API_INIT;

   return cdds_openm(*out_bin_tag, *out_sym_tag,
                      *in_bin_tag,  *in_sym_tag);
}
