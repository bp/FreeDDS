static char rcsid[]="$Id: f_index.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, fetch field index from DDS structures

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fdds_index, FDDS_INDEX)(
   HOST_FTN_INT *bin,
   HOST_FTN_INT *field,
   HOST_FTN_INT *prime)
{
   DDS_API_INIT;

   return cdds_index(*bin, *field, *prime);
}
