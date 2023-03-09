static char rcsid[]="$Id: dumpbinsym.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_BIN and associated symbol tables

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpBinSym(
   int indent,
   BIN_TAG bin_tag)
{
   DDS_BIN *bin;

   if(! dds_initialized) return; /* no-op */

   bin = dds_bin[bin_tag];

   if(! bin || bin_tag != bin->tag) {
      fprintf(dds_print_fp, "ddsDumpBinSym: BAD bin tag= %i\n", (int)bin_tag);
      return;
   }

   fprintf(dds_print_fp, "bin tag= %i; alias= %s; name= %s;\n",
      (int)bin->tag,
      bin->alias_name,
      bin->name);

   fprintf(dds_print_fp, "%*sblock_old= %p; new= %p;\n",
      indent, "",
      (void*)bin->block_old,
      (void*)bin->block_new);

   fprintf(dds_print_fp, "%*s", indent, "");

   ddsDumpTable(indent + DDS_DENT, &bin->symbol);

   fprintf(dds_print_fp, "%*s", indent, "");

   ddsDumpMapOut(indent + DDS_DENT, bin_tag);

   fprintf(dds_print_fp, "%*s", indent, "");

   ddsDumpBlockR(DDS_TRUE, indent + DDS_DENT, (DDS_BLOCK*)bin->symbol.oldest);

   return;
}
