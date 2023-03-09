static char rcsid[]="$Id: strfree.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, free DDS_STRING buffer.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsStrFree(
   DDS_STRING *str)
{
   if(str->ptr) ddsFree(str->ptr);
   str->ptr = NULL;
   str->max = 0;

   return str;
}

