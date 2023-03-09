static char rcsid[]="$Id: fx_read.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, convenience routine for reading data

*/

/***************************************/
/***   Copyright (C) 2000            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fddx_read, FDDX_READ)(
   HOST_FTN_INT *bin_tag,
   HOST_FTN_PTR  buf,
   HOST_FTN_INT *count
   )
{
   DDS_API_INIT;

   return cddx_read(*bin_tag, buf, *count);
}
