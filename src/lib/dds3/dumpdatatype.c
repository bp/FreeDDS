static char rcsid[]="$Id: dumpdatatype.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump data buffer, given its DDS_TYPE

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpDataType(
   int indent,
   DDS_TYPE *type,
   size_t offset,
   const char *buf,
   const char *buf1,
   const char *buf2)
{
   DDS_BLOCK *block;
   char str[DDS_DUMP_MAX + 1];

   if(! dds_initialized) return; /* no-op */

   if(! type) {
      fprintf(dds_print_fp, "ddsDumpDataType: NULL TYPE ptr\n");
      return;
   }

   assert(type->flag & DDS_SYM_TYPE);

   if(type->flag & DDS_SYM_ARRAY) {
      if(type->count) {
         /* array size greater than zero */
         ddsDumpDataArray(indent, type, offset,
            buf, buf1, buf2);
      } else {
         /* array size is zero (collapsed) */
         while(type->flag & DDS_SYM_ARRAY) {
            sprintf(str, "[0 of %lu]=",
               (unsigned long)type->count);
            ddsStackChar(&dds_dump_que, str, strlen(str));
            type = type->type;
         }
         sprintf(str, "= <collapsed>\n");
         ddsStackChar(&dds_dump_que, str, strlen(str));
      }
   } else {
      if(type->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
         block = type->block;
         ddsDumpDataBlock(indent, block, offset,
            buf, buf1, buf2);
      } else if(type->ptype == DDS_CTYPE_VOID) {
         ddsStackChar(&dds_dump_que, "= void\n", 7);
      } else {
         ddsStackChar(&dds_dump_que, "= ", 1);
         ddsDumpDataValue(1, 0, type, buf + offset);
         ddsStackChar(&dds_dump_que, "\n", 1);
      }
   }
   return;
}
