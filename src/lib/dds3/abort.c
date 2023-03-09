static char rcsid[]="$Id: abort.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, abort process execution.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsAbort(void)
{
   /* abort();  ouch, abort does NOT return a non-zero status */
   exit(1);
   return;
}

