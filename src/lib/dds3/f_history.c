static char rcsid[]="$Id: f_history.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, copy all scan dictionaries to print dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_history, FDDS_HISTORY)()
{
   DDS_API_INIT;

   return cdds_history();
}
