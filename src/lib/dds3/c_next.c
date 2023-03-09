static char rcsid[]="$Id: c_next.c 114 2009-09-09 20:43:08Z user $";
/*
RCS-t=DDS API, return next symbol tag (member or object)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 11 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

SYM_TAG (cdds_next)(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
   DDS_BIN *bin;
   DDS_SYM *sym;
   DDS_SYM_FLAG flag;
   SYM_TAG new_tag, tag;
#if defined(_WIN32) || defined(MINGW)
   size_t *bias_ptr;
#else
   long *bias_ptr;
#endif
   DDS_MEMBER *member;
   DDS_BLOCK *block;
   const char *name;
 
   DDS_API_ENTRY("next");
 
   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD)); 

   SYM_TAG_TO_SYM(sym, bin, sym_tag, DDS_API_EXIT(DDS_BAD));
    
   flag = (DDS_SYM_FLAG)((sym->flag & DDS_SYM_MEMBER) |
      (sym->flag & DDS_SYM_OBJECT));

   new_tag = (SYM_TAG)-1;
   if (flag) {
      for(sym = (DDS_SYM*)sym->newer;
         sym;
         sym = (DDS_SYM*)sym->newer) {
         if(sym->flag & flag) break;
      }
      SYM_TO_SYM_TAG(new_tag, &bin->sym_tag, sym);

      /* This is needed to setup the bias */
      if (new_tag >= 0) {
         SYM_TAG_TO_BIAS(bias_ptr, bin, new_tag, DDS_API_EXIT(DDS_BAD))
         *bias_ptr = 0;
         tag = new_tag;
         while (tag >= 0) {
            SYM_TAG_TO_SYM(sym, bin, tag, DDS_API_EXIT(DDS_BAD));
            name = sym->name;
            if (!name) name = sym->newer->name;
            if (!name) break;
            
            member = UNIQUE_MEMBER(&bin->symbol, name, sym->block);
            if (!member) break;

            *bias_ptr += member->offset;
            tag = cdds_parent(bin_tag, tag);
         }
      }
   }

   DDS_API_EXIT(new_tag)
}
