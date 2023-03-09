static char rcsid[]="$Id: symblockbegin.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, begin nested DDS_BLOCK ("tag" name) on symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_BLOCK * ddsSymBlockBegin(
   const char *name,
   DDS_SYM_FLAG flag,
   DDS_TYPE *merge_type)
{
   DDS_TYPE *type;
   DDS_BLOCK *block, *reblock;
   DDS_BOOLEAN redefined = DDS_FALSE;
   const char *name_ptr = NULL;

   if(dds_block) {
      /* this is *not* the initial block within a symbol table */
      if(name) {
         reblock = UNIQUE_BLOCK(dds_symbol, name, dds_block);
         if(reblock) {
            ddsYYError("'%s' tag already defined in the current scope\n",
               name);
            redefined = DDS_TRUE;
            name_ptr = reblock->name;
         } else {
            name_ptr = ddsTablePushStr(dds_symbol, name);
         }
      }
   } else if(name) {
      name_ptr = ddsTablePushStr(dds_symbol, name);
   }

   block = PUSH_BLOCK(dds_symbol, name_ptr, flag,
      dds_block, NULL);

   /* new block becomes the new local block */
   dds_block = block;

   if(merge_type) {
      type = ddsSymTypeMerge(merge_type);
      if(flag & DDS_SYM_ENUM) {
         type->flag |= (DDS_SYM_FLAG)(DDS_SYM_ENUM |
            DDS_SYM_CONST | DDS_SYM_DECL);
      } else {
         type->flag |= (DDS_SYM_FLAG)(flag & DDS_SYM_SUE);
      }
   } else {
      type = NULL;
   }

   block->type = type;

   if(redefined) {
      block->flag |= (DDS_SYM_FLAG)(DDS_SYM_ERROR | DDS_SYM_MULTIPLE);
      block->hash = ~ block->hash; /* name will never match */
      if(type) type->flag |= DDS_SYM_ERROR;
   }

   DECLARE_INNER;

   return block;
}
