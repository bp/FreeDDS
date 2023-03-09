static char rcsid[]="$Id: symconsttype.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, test DDS_TYPE for 'const' qualifiers (recursive)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_BOOLEAN ddsSymConstType(
   DDS_TYPE *type)
{
   DDS_SYM_FLAG flag;
   DDS_SYM *sym;

   flag = type->flag;

   if(flag & DDS_SYM_CONST) return DDS_TRUE;

   if(flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
      /* test each member of struct/union */
      for(sym = type->block->first; sym; sym = (DDS_SYM*)sym->newer) {
         if((sym->flag & DDS_SYM_MEMBER) &&
            ddsSymConstType(sym->type)) return DDS_TRUE;
      }
   }

   return DDS_FALSE;
}
