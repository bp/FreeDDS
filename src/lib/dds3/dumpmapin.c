static char rcsid[]="$Id: dumpmapin.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_MAP for an input binary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpMapIn(
   int indent,
   BIN_TAG in_bin_tag)
{
   DDS_MAP *map;
   BIN_TAG out_bin_tag;
   DDS_BIN *out_bin, *in_bin;
   const char *name;

   if(! dds_initialized) return; /* no-op */

   in_bin = dds_bin[in_bin_tag];

   if(! in_bin) {
      fprintf(dds_print_fp, "ddsDumpMapIn: NULL IN_BIN ptr\n");
      return;
   }

   fprintf(dds_print_fp, "map input alias= %s;\n", in_bin->alias_name);

   for(out_bin_tag = 0; out_bin_tag < dds_bin_max; out_bin_tag++) {
      map = MAP_PTR(out_bin_tag, in_bin_tag);
      if(map) {
         out_bin = dds_bin[out_bin_tag];
         fprintf(dds_print_fp, "%*soutput alias= %s;\n",
            indent, "", out_bin->alias_name);
         while(map) {
            name = map->out_object->block->name;
            fprintf(dds_print_fp, "%*sname= %s;\n",
               indent + DDS_DENT, "", name);
            fprintf(dds_print_fp, "%*sout_object= %p; in_object= %p;\n",
               indent + DDS_DENT, "", (void*)map->out_object,
                                      (void*)map->in_object);
            fprintf(dds_print_fp, "%*sthread= %p; needle= %p\n",
               indent + DDS_DENT, "", (void*)map->seam.thread,
                                      (void*)map->seam.needle);
            map = map->next;
         }
      }
   }
   return;
}
