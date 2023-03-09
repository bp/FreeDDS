static char rcsid[]="$Id: f_axis_prod.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, query axis product 

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_axis_prod, FDDS_AXIS_PROD)(
   HOST_FTN_INT *rank)
{
   DDS_API_INIT;

   return cdds_axis_prod(*rank);
}
