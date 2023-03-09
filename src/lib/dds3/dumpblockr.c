static char rcsid[]="$Id: dumpblockr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_BLOCK recursively

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpBlockR(
   DDS_BOOLEAN init,
   int indent,
   DDS_BLOCK *block)
{
   DDS_SYM *sym;

   if(! dds_initialized) return; /* no-op */

   if(! block) {
      fprintf(dds_print_fp, "ddsDumpBlockR: NULL BLOCK ptr\n");
      return;
   }

   fprintf(dds_print_fp, "BLOCK ");
   ddsDumpSym(indent, (DDS_SYM*)block);

   fprintf(dds_print_fp, "%*sfirst= %p; last= %p;\n",
      indent, "", block->first,
                  block->last);

   for(sym = block->first;
      sym;
      sym = (DDS_SYM*)sym->newer) {
      fprintf(dds_print_fp, "%*s", indent, "");
      ddsDumpBlockRS(indent + DDS_DENT, sym);
   }

   if(init && ! block->first && block->newer) {
      fprintf(dds_print_fp, "%*sOPEN symbols\n", indent, "");

      for(sym = (DDS_SYM*)block->newer;
         sym;
         sym = (DDS_SYM*)sym->newer) {
         fprintf(dds_print_fp, "%*s", indent + DDS_DENT, "");
         ddsDumpBlockRS(indent + 2 * DDS_DENT, sym);
      }
   }

   return;
}
