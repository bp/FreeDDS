static char rcsid[]="$Id: dumpdatalist.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump data buffer, given a list of headers

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpDataList(
   DDS_BIN *bin,
   const char *mode,
   DDS_TYPE *type,
   const char *buf)
{
   static unsigned list_lines = 0;
   DDS_DUMP_DIR *dir;
   DDS_DUMP_ITEM *item;
   int n;

   if(! dds_initialized) return; /* no-op */

   if(! type) {
      fprintf(dds_print_fp, "ddsDumpDataList: NULL TYPE ptr\n");
      return;
   }

   dir = dds_dump_dir;
   item = bin->dump_item;

   if(! (list_lines++ % 50)) {
      fprintf(dds_print_fp, "\nDDS: Header ");
      for(n = 0; n < dds_dump_max; n++) {
         fprintf(dds_print_fp, " %*s", dir[n].width, dir[n].name);
      }
      fprintf(dds_print_fp, "\n\n");
   }

   fprintf(dds_print_fp, "DDS:%5s(%i)", mode, (int)bin->tag);

   if(item && buf) {
      for(n = 0; n < dds_dump_max; n++) {
         type = item[n].type;
         ddsDumpDataValue(dir[n].width, dir[n].width,
            type, buf + item[n].offset);
      }
   }
   fprintf(dds_print_fp, "\n");

   return;
}
