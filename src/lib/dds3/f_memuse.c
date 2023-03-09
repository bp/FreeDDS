static char rcsid[]="$Id: f_memuse.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, malloc memory with error detection.

*/

/**********************************************/
/***   Copyright (C) 2006                   ***/
/***   BP America, Inc, Houston, TX         ***/
/***   Jerry Ehlers, 10 Oct 2006            ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_DBL
HOST_FTN_FUNC(fdds_memuse, FDDS_MEMUSE)()
{
   DDS_API_INIT;

   return (HOST_FTN_DBL)cdds_memuse();
}
