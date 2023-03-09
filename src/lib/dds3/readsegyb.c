static char rcsid[]="$Id: readsegyb.c 211 2011-01-11 14:32:42Z user $";
/*
RCS-t=DDS API, read segy binary header binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsReadSegyB(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
{
   if (count != 1) {
      ddsError("Can only read 1 segy binary preamble, not count=%d\n\tbinary= %s\n",
               count, bin->name);
      return(0);
   }

   memcpy(buffer, bin->std.segy.binary_buf, type->stroke);
   return(count);
}
