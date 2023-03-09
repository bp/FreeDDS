static char rcsid[]="$Id: dumpblock.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_BLOCK

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpBlock(
   int indent,
   DDS_BLOCK *block)
{
   if(! dds_initialized) return; /* no-op */

   if(! block) {
      fprintf(dds_print_fp, "ddsDumpBlock: NULL BLOCK ptr\n");
      return;
   }

   fprintf(dds_print_fp, "BLOCK ");
   ddsDumpSym(indent, (DDS_SYM*)block);

   fprintf(dds_print_fp, "%*sfirst= %p; last= %p;\n",
      indent, "", block->first,
                  block->last);

   return;
}
