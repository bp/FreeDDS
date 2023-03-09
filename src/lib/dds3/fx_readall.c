static char rcsid[]="$Id: fx_readall.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, convenience read entire dataset

*/

/***************************************/
/***   Copyright (C) 2000            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fddx_readall, FDDX_READALL)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_PTR *buffer,
   HOST_FTN_INT *rank,
   HOST_FTN_INT *size,
   HOST_FTN_REAL *delta,
   HOST_FTN_REAL *origin,
   HOST_FTN_LEN alias_len
   )
{
   static DDS_STRING alias_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   return cddx_readall(alias_str.ptr, buffer, rank, size, delta, origin);
}
