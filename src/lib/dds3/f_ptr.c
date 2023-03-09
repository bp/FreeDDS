static char rcsid[]="$Id: f_ptr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, return address of input argument

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, 08 Nov 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_PTR 
HOST_FTN_FUNC(fdds_ptr, FDDS_PTR)(
   HOST_FTN_PTR  arg)                /* input arg */
{
   /* return address of input argument */

   DDS_API_INIT;

   return (HOST_FTN_PTR)arg;
}
