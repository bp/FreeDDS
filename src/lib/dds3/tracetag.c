static char rcsid[]="$Id: tracetag.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, find trace SEQUENCE and resolve it's type

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsTraceTag(
   DDS_BIN *bin)
{
   if(ddsObjectTag(&bin->trace, bin->block_new, "SEQUENCE")) {
      ddsError("Bogus '%s' format SEQUENCE\n\tbinary= %s\n",
         bin->fmt_name, bin->name);
      return DDS_BAD;
   }

   /* default type */
   ((DDS_SYM_TAG*)bin->sym_tag.ptr)[0] =
      ((DDS_SYM_TAG*)(bin->sym_tag.ptr))[bin->trace.object->tag];
/*** bad idea
   ((DDS_SYM_TAG*)bin->sym_tag.ptr)[0] =
      ((DDS_SYM_TAG*)(bin->sym_tag.ptr))[bin->trace.type->tag];
***/

   return DDS_OK;
}
