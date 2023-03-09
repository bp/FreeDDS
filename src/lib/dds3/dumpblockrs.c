static char rcsid[]="$Id: dumpblockrs.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_BLOCK recursive symbol

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpBlockRS(
   int indent,
   DDS_SYM *sym)
{
   if(! dds_initialized) return; /* no-op */

   if(! sym) {
      fprintf(dds_print_fp, "ddsDumpBlockRS: NULL SYM ptr\n");
      return;
   }

   if(sym->flag & DDS_SYM_BLOCK) {
      ddsDumpBlockR(DDS_FALSE, indent, (DDS_BLOCK*)sym);
   } else if(sym->flag & DDS_SYM_OBJECT) {
      ddsDumpObject(indent, (DDS_OBJECT*)sym);
   } else if(sym->flag & DDS_SYM_MEMBER) {
      ddsDumpMember(indent, (DDS_MEMBER*)sym);
   } else if(sym->flag & DDS_SYM_TYPE) {
      ddsDumpType(indent, (DDS_TYPE*)sym);
   } else if(sym->flag & DDS_SYM_EXPR) {
      ddsDumpExpr(indent, (DDS_EXPR4*)sym);
   } else {
      fprintf(dds_print_fp, "SYM %p ", sym);
      ddsDumpSym(indent, sym);
   }

   return;
}
