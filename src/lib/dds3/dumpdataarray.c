static char rcsid[]="$Id: dumpdataarray.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump data buffer, given its DDS_TYPE array

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpDataArray(
   int indent,
   DDS_TYPE *type,
   size_t offset,
   const char *buf,
   const char *buf1,
   const char *buf2)
{
   DDS_TYPE *typea;
   DDS_TYPE_GENUS type_genus;
   char str[DDS_DUMP_MAX + 1];
   int i, j, n, count;

   if(! dds_initialized) return; /* no-op */

   if(! type) {
      fprintf(dds_print_fp, "ddsDumpDataArray: NULL TYPE ptr\n");
      return;
   }

   assert(type->flag & DDS_SYM_TYPE);
   assert(type->flag & DDS_SYM_ARRAY);

   count = (type->count < dds_dump_max) ? type->count : dds_dump_max;
   type_genus = dds_prime[type->ptype].genus;

   if(offset == 0) {
      /* beginning of array */
      /* dump subscript range. */
      sprintf(str, "[%i of %lu]", count, (unsigned long)type->count);
      ddsStackChar(&dds_dump_que, str, strlen(str));
   }

   typea = type->type;
   if(typea->flag & DDS_SYM_ARRAY) {
      /* this is not the last dimension. */
      for(n = 0; n < count; n++) {
         /* dump each array dimension (recursive) */
         ddsDumpDataArray(indent, typea,
            offset + n * type->prec,
            buf, buf1, buf2);
         if(typea->ptype == DDS_CTYPE_VOID) break;
      }
   } else if(typea->ptype == DDS_CTYPE_VOID) {
      /* this is the last dimension (void type) */
      ddsStackChar(&dds_dump_que, "= void\n", 7);
   } else {
      /* this is the last dimension (non-void type) */
      if(typea->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
         /* print array contents (struct/union) */
         for(i = 0; i < count; i++) {
            ddsDumpDataType(indent + DDS_DENT, typea, offset,
               buf, buf1, buf2);
            offset += typea->prec;
         }
      } else {
         /* print array contents (non struct/union) */
         if(count <= dds_prime[type->ptype].dump / 2) {
            /* print contents on current line */
            ddsStackChar(&dds_dump_que, "=", 1);
         } else {
            /* print contents on another line */
            ddsStackChar(&dds_dump_que, "=\n", 2);
            ddsDumpDataDiff(indent, type, 0,
               NULL, NULL, NULL);
         }
         if(type_genus == DDS_TYPE_CHARS) {
            /* print character array contents */
            for(i = 0; i < count; i += j) {
               if(i) {
                  /* print contents on another line */
                  ddsDumpDataDiff(indent, type, offset,
                     NULL, NULL, NULL);
               }
               if(i + dds_prime[type->ptype].dump > count) {
                  j = count - i;
               } else {
                  j = dds_prime[type->ptype].dump;
               }
               ddsDumpDataValue(0, j, type, buf);
               buf += j;
               offset += j;
            }
         } else {
            /* print other array contents */
            for(i = 0; i < count; i++) {
               if(i && ! (i % dds_prime[type->ptype].dump)) {
                  /* print contents on another line */
                  ddsStackChar(&dds_dump_que, "\n", 1);
                  ddsDumpDataDiff(indent, type, offset,
                     NULL, NULL, NULL);
               }
               ddsDumpDataValue(1, 0, type, buf);
               buf += type->prec;
               offset += type->prec;
            }
         }
         /* print final newline */
         ddsStackChar(&dds_dump_que, "\n", 1);
      }
   }
   return;
}
