static char rcsid[]="$Id: f_initmpi.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, initialize MPI

*/

/**********************************************/
/***   Copyright (C) 2007                   ***/
/***   BP America, Inc, Houston, Texas      ***/
/***   Jerry Ehlers, 24 FEB 2006            ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_initmpi, FDDS_INITMPI)(
   HOST_FTN_INT *node,
   HOST_FTN_INT *nodes)
{
   DDS_API_INIT;

   return cdds_initmpi(node, nodes);
}


HOST_FTN_INT
HOST_FTN_FUNC(fdds_initmpix, FDDS_INITMPIX)(
   HOST_FTN_INT *node,
   HOST_FTN_INT *nodes)
{
   DDS_API_INIT;

   return cdds_initmpix(node, nodes);
}
