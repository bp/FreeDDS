static char rcsid[]="$Id: f_putff.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, put fake float

*/

/****************************************/
/***   Copyright (C) 2004             ***/
/***   BP America, Houston, TX        ***/
/***   Jerry Ehlers, 30 March 2004    ***/
/****************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_putff, FDDS_PUTFF)(
   HOST_FTN_INT *bin,
   HOST_FTN_INT *field,
   HOST_FTN_PTR  buf,
   HOST_FTN_INT *offset,
   HOST_FTN_REAL *array,
   HOST_FTN_INT *count)
{
   DDS_API_INIT;

   return cdds_putff(*bin, *field, (char*)buf + *offset, array, *count);
}
