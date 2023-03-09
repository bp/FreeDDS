static char rcsid[]="$Id: f_getff.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, get fake float

*/

/****************************************/
/***   Copyright (C) 2004             ***/
/***   BP America, Houston, TX        ***/
/***   Jerry Ehlers, 30 March 2004    ***/
/****************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_getff, FDDS_GETFF)(
   HOST_FTN_INT *bin,
   HOST_FTN_INT *field,
   HOST_FTN_PTR  buf,
   HOST_FTN_INT *offset,
   HOST_FTN_REAL *array,
   HOST_FTN_INT *count)
{
   DDS_API_INIT;

   return cdds_getff(*bin, *field, (char*)buf + *offset, array, *count);
}
