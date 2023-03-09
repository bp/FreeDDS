static char rcsid[]="$Id: dumps.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump Simple DDS_SYM symbol

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpS(
   DDS_SYM *sym)
{
   if(! dds_initialized) return; /* no-op */

   if(! sym) {
      fprintf(dds_print_fp, "ddsDumpS: NULL SYM ptr\n");
      return;
   }

   if(sym->flag & DDS_SYM_BLOCK) {
      ddsDumpBlock(0, (DDS_BLOCK*)sym);
   } else if(sym->flag & DDS_SYM_OBJECT) {
      ddsDumpObject(0, (DDS_OBJECT*)sym);
   } else if(sym->flag & DDS_SYM_MEMBER) {
      ddsDumpMember(0, (DDS_MEMBER*)sym);
   } else if(sym->flag & DDS_SYM_TYPE) {
      ddsDumpType(0, (DDS_TYPE*)sym);
   } else if(sym->flag & DDS_SYM_EXPR) {
      ddsDumpExpr(0, (DDS_EXPR4*)sym);
   } else {
      fprintf(dds_print_fp, "SYM %p ", sym);
      ddsDumpSym(0, sym);
   }

   return;
}
