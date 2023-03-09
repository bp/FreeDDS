static char rcsid[]="$Id: f_free.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, free memory with error detection.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_free, FDDS_FREE)(
   HOST_FTN_PTR *ptr)
{
   DDS_API_INIT;

   cdds_free((void*)*ptr);
   return 0;
}
