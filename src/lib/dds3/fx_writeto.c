static char rcsid[]="$Id: fx_writeto.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, convenience routine for writing data

*/

/***************************************/
/***   Copyright (C) 2000            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fddx_writeto, FDDX_WRITETO)(
   HOST_FTN_INT *bin_tag,
   HOST_FTN_INT *offset,
   HOST_FTN_PTR  buf,
   HOST_FTN_INT *count
   )
{
   DDS_API_INIT;

   return cddx_writeto(*bin_tag, *offset, buf, *count);
}
