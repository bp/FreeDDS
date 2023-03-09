static char rcsid[]="$Id: dumpitem.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_ITEM

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpItem(
   int indent,
   DDS_ITEM *item)
{
   DDS_HASH hash;
   const char *name;
   char mash;

   if(! dds_initialized) return; /* no-op */

   if(! item) {
      fprintf(dds_print_fp, "ddsDumpItem: NULL ITEM ptr\n");
      return;
   }

   if(item->name) {
      name = item->name;
      hash = ddsHash(name);
   } else {
      name = "";
      hash = 0;
   }

   if(hash == item->hash)
      mash = '#';
   else
      mash = '?';

   fprintf(dds_print_fp, "%p name='%s'%c;\n",
      (void*)item,
      name,
      mash);

   fprintf(dds_print_fp, "%*schain= %p; older= %p; newer= %p;\n",
      indent, "",
                  (void*)item->chain,
                  (void*)item->older,
                  (void*)item->newer);

   return;
}
