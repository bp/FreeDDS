static char rcsid[]="$Id: dumpdeclare.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_DECLARE

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpDeclare(
   int indent,
   DDS_DECLARE *declare)
{
   DDS_SYM sym;

   if(! dds_initialized) return; /* no-op */

   if(! declare) {
      fprintf(dds_print_fp, "ddsDumpDeclare: NULL DECLARE ptr\n");
      return;
   }

   if(declare->outer) {
      ddsDumpDeclare(indent, declare->outer);
      fprintf(dds_print_fp, "%*s", indent, "");
   }

   fprintf(dds_print_fp, "DECLARE &= %p; ", (void*)declare);
   if(! declare->outer)
      fprintf(dds_print_fp, "OLDEST ");
   else
      fprintf(dds_print_fp, "NEWER ");
   sym.flag = declare->flag;
   ddsDumpSymFlag(indent, &sym);
   fprintf(dds_print_fp, "%*stype= %p; more= %p\n",
      indent + DDS_DENT, "", (void*)declare->type,
                             (void*)declare->more);

   return;
}
