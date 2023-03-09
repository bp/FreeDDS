static char rcsid[]="$Id: dumpsym.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_SYM

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpSym(
   int indent,
   DDS_SYM *sym)
{
   if(! dds_initialized) return; /* no-op */

   if(! sym) {
      fprintf(dds_print_fp, "ddsDumpSym: NULL SYM ptr\n");
      return;
   }

   ddsDumpItem(indent, (DDS_ITEM*)sym);

   fprintf(dds_print_fp, "%*stag= %i; ",
      indent, "", (int)sym->tag);
   ddsDumpSymFlag(indent + DDS_DENT, sym);

   fprintf(dds_print_fp, "%*sblock= %p; type= %p;\n",
      indent, "", (void*)sym->block,
                  (void*)sym->type);

   return;
}
