static char rcsid[]="$Id: c_prev.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, return previous symbol tag (member or object)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 11 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

SYM_TAG (cdds_prev)(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
   DDS_BIN  *bin;
   DDS_SYM *sym;
   DDS_BLOCK *block;
   DDS_TYPE *type;
   SYM_TAG new_tag;
 
   DDS_API_ENTRY("prev");
 
   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD)); 

   SYM_TAG_TO_SYM(sym, bin, sym_tag, DDS_API_EXIT(DDS_BAD));
    
   switch(sym->block->flag & DDS_SYM_GENUS) {
   case DDS_SYM_MEMBER:
      for(sym = (DDS_SYM*)sym->older;
         sym;
         sym = (DDS_SYM*)sym->older) {
         if(sym->flag & DDS_SYM_MEMBER) break;
      }
      break;
   case DDS_SYM_OBJECT:
      for(sym = (DDS_SYM*)sym->older;
         sym;
         sym = (DDS_SYM*)sym->older) {
         if((sym->flag & DDS_SYM_OBJECT) && sym->name) break;
      }
      break;
   case DDS_SYM_TYPE:
   case DDS_SYM_BLOCK:
   case DDS_SYM_EXPR:
   case DDS_SYM_LABEL:
   default:
      ddsPanic("cdds_older: don't know what to do\n");
   }

   SYM_TO_SYM_TAG(new_tag, &bin->sym_tag, sym)
 
   DDS_API_EXIT(new_tag)
}
