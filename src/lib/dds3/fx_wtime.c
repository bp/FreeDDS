static char rcsid[]="$Id: fx_wtime.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, convenience routine for wall-time
*/

/***************************************/
/***   Copyright (C) 2008            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, Oct 2008        ***/
/***************************************/

#include "dds_sys.h"


HOST_FTN_DBL
HOST_FTN_FUNC(fddx_wtime, FDDX_WTIME)()
{
   DDS_API_INIT;

   return cddx_wtime();
}
