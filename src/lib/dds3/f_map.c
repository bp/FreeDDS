static char rcsid[]="$Id: f_map.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, map binary data to output buffer from input buffer

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_map, FDDS_MAP)(
   HOST_FTN_INT *out_bin_tag,
   HOST_FTN_INT *out_sym_tag,
   HOST_FTN_PTR  out_buf,
   HOST_FTN_INT *in_bin_tag,
   HOST_FTN_INT *in_sym_tag,
   HOST_FTN_PTR  in_buf)
{
   DDS_API_INIT;

   return cdds_map(*out_bin_tag, *out_sym_tag, out_buf,
                    *in_bin_tag,  *in_sym_tag,  in_buf);
}
