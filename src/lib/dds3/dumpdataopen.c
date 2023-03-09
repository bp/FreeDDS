static char rcsid[]="$Id: dumpdataopen.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open dump data buffer, given a list of headers

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpDataOpen(
   DDS_BIN *bin)
{
   DDS_DUMP_DIR *dir;
   DDS_DUMP_ITEM *item;
   DDS_TYPE *type;
   DDS_MEMBER *member;
   int n;

   if(! dds_initialized) return; /* no-op */

   if(! bin) {
      fprintf(dds_print_fp, "ddsDumpDataOpen: NULL BIN ptr\n");
      return;
   }

   if(dds_state & DDS_DUMP_LIST) {
      /* process dump directory */
      dir = dds_dump_dir;
      item = (DDS_DUMP_ITEM*)ddsZalloc(dds_dump_max *
         sizeof(DDS_DUMP_ITEM));
      bin->dump_item = item;

      type = bin->trace.type;
      if(type->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
         for(n = 0; n < dds_dump_max; n++) {
            member = ddsSymLookupMem(&bin->symbol, dir[n].name, type->block,
               &item[n].offset);
            if(member) {
               item[n].type = member->type;
            }
         }
      }
   } else bin->dump_item = (DDS_DUMP_ITEM*)0;

   return;
}
