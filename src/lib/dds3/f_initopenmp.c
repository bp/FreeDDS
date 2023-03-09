static char rcsid[]="$Id: f_initopenmp.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, initialize OpenMP

*/

/**********************************************/
/***   Copyright (C) 2006                   ***/
/***   BP America, Inc, Houston, Texas      ***/
/***   Jerry Ehlers, 24 FEB 2006            ***/
/**********************************************/

#include "dds_sys.h"

extern int cdds_initopenmp();

HOST_FTN_INT
HOST_FTN_FUNC(fdds_initopenmp, FDDS_INITOPENMP)()
{
   DDS_API_INIT;

   return cdds_initopenmp();
}
