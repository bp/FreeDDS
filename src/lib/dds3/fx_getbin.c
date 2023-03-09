static char rcsid[]="$Id: fx_getbin.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, convenience function to get the associated i/o binary tag

*/

/**********************************************/
/***   Copyright (C) 2001                   ***/
/***   BP Corporation, Houston, TX          ***/
/***   Jerry Ehlers, 13 March 2001          ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fddx_getbin, FDDX_GETBIN)(
   HOST_FTN_INT *bin_tag
   )
{
   DDS_API_INIT;

   return cddx_getbin(*bin_tag);
}
