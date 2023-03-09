static char rcsid[]="$Id: f_prime.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, fetch field prime tag from DDS structures

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 15 July 1996           ***/
/**********************************************/

#include "dds_sys.h"


HOST_FTN_INT
HOST_FTN_FUNC(fdds_prime, FDDS_PRIME)(
   HOST_FTN_INT *bin,
   HOST_FTN_INT *field)
{
   DDS_API_INIT;

   return cdds_prime(*bin, *field);
}
