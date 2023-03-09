static char rcsid[]="$Id: readuspl.c 219 2011-02-16 01:11:40Z user $";
/*
RCS-t=DDS API, read usp line header binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsReadUspL(
   DDS_BIN *bin,
   DDS_TYPE *type,
   char *buffer,
   int count)
{
   if (count != 1) {
      ddsError("Can only read 1 usp binary preamble, not count=%d\n\tbinary= %s\n",
               count, bin->name);
      return(0);
   }

   memcpy(buffer, bin->std.usp.line_buf, type->stroke);
   return(count);
}
