static char rcsid[]="$Id: fx_rmaxis.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, remove an existing axis from a dictionary.

*/

/***************************************/
/***   Copyright (C) 2001            ***/
/***   BP Corporation, Houston, TX   ***/
/***   Jerry Ehlers, July 2001       ***/
/***************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fddx_rmaxis, FDDX_RMAXIS)(
   HOST_FTN_INT *bin_tag,
   HOST_FTN_INT *num
   )
{
   DDS_API_INIT;

   return cddx_rmaxis(*bin_tag, *num);
}
