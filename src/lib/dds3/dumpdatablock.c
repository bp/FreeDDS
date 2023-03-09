static char rcsid[]="$Id: dumpdatablock.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump data buffer, given its DDS_BLOCK

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpDataBlock(
   int indent,
   DDS_BLOCK *block,
   size_t offset,
   const char *buf,
   const char *buf1,
   const char *buf2)
{
   DDS_MEMBER *member;
   DDS_SYM *sym;
   DDS_TYPE *type;
   size_t top;
   size_t bias;
   int rc;

   if(! dds_initialized) return; /* no-op */

   if(! block) {
      fprintf(dds_print_fp, "ddsDumpDataBlock: NULL BLOCK ptr\n");
      return;
   }

   assert(block->type && "ddsDumpDataBlock is confused");

   type = block->type;

   if(type->flag & DDS_SYM_STRUCT) {
      ddsStackChar(&dds_dump_que, "= struct\n", 9);
   } else if(type->flag & DDS_SYM_UNION) {
      ddsStackChar(&dds_dump_que, "= union\n", 8);
   } else {
      ddsPanic("ddsDumpDataBlock: bad block\n");
   }

   for(sym = block->first;
      sym;
      sym = (DDS_SYM*)sym->newer) {
      if(! (sym->flag & DDS_SYM_MEMBER)) continue;
      member = (DDS_MEMBER*)sym;
      assert(member->block == block);
      type = member->type;
      bias = offset + member->offset;
      top = dds_dump_que.top;
      rc = ddsDumpDataDiff(indent, type, bias,
         buf, buf1, buf2);
      if(rc == DDS_OK) {
         ddsStackChar(&dds_dump_que, member->name, strlen(member->name));
         if(type->flag & (DDS_SYM_ARRAY | DDS_SYM_STRUCT | DDS_SYM_UNION)) {
            ddsDumpDataType(indent + DDS_DENT, type, 0,
               buf + bias, buf1 + bias, buf2 + bias);
         } else {
            ddsStackChar(&dds_dump_que, "=", 1);
            ddsDumpDataValue(1, 1, type, buf + bias);
            ddsStackChar(&dds_dump_que, "\n", 1);
         }
      } else {
         /* muted dump for this branch */
         dds_dump_que.ptr[top] = '\0';
         dds_dump_que.top = top;
      }
   }
   return;
}
