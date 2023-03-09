static char rcsid[]="$Id: f_check.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, check debug flags

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_check, FDDS_CHECK)(
   HOST_FTN_INT *flag)
{
   DDS_API_INIT;

   return cdds_check((DDS_DEBUG)*flag);
}
