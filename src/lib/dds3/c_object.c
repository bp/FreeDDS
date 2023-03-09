static char rcsid[]="$Id: c_object.c 16 2009-01-06 21:52:47Z user $";
/*
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 April 1997          ***/
/**********************************************/

#include "dds_sys.h"

SYM_TAG (cdds_object)(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const char *field_name)
{
   DDS_BIN *bin;
   DDS_SYM *sym;
   DDS_BLOCK *block;
   DDS_OBJECT *object;
   SYM_TAG new_tag;

   DDS_API_ENTRY("object");

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_SYM(sym, bin, sym_tag, DDS_API_EXIT(DDS_BAD))

   if ((sym->flag & DDS_SYM_BLOCK) && 
      !(sym->flag & DDS_SYM_STRUCT) && 
      !(sym->flag & DDS_SYM_UNION) && 
      !(sym->flag & DDS_SYM_ENUM)) {
      block = (DDS_BLOCK*)sym;
   } else {
      block = NULL;
   }

   if (block) {
      if (field_name) while (isspace(*field_name)) field_name++;
      if (field_name && *field_name) {
         object = UNIQUE_OBJECT(&bin->symbol, field_name, block);
      } else {
         for(object = (DDS_OBJECT*)block->newer; 
            object; 
            object=(DDS_OBJECT*)object->newer) { 
            if ((object->flag & DDS_SYM_OBJECT) && 
               (object->block == block)) break;             
         }
      }
      SYM_TO_SYM_TAG(new_tag, &bin->sym_tag, object)
   } else new_tag = (SYM_TAG)-1;

   DDS_API_EXIT(new_tag);
}
