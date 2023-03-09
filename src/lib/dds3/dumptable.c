static char rcsid[]="$Id: dumptable.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_TABLE

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpTable(
   int indent,
   DDS_TABLE *table)
{
   if(! dds_initialized) return; /* no-op */

   if(! table) {
      fprintf(dds_print_fp, "ddsDumpTable: NULL TABLE ptr\n");
      return;
   }

   fprintf(dds_print_fp, "TABLE &= %p; chain= %p; oldest= %p; newest= %p;\n",
      (void*)table,
      (void*)table->chain,
      (void*)table->oldest,
      (void*)table->newest);

   fprintf(dds_print_fp, "%*ssize= %5li; load= %5li; count= %5li; unique= %5li;\n",
      indent, "", (long)table->size,
                  (long)table->load,
                  (long)table->count,
                  (long)table->unique);

   fprintf(dds_print_fp, "%*salpha= %5f; beta= %5f;\n",
      indent, "", (double)table->alpha,
                  (double)table->beta);

   fprintf(dds_print_fp, "%*s", indent, "");
   ddsDumpPage(indent, &table->page);

   return;
}
