static char rcsid[]="$Id: dumptype.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_TYPE

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpType(
   int indent,
   DDS_TYPE *type)
{
   if(! dds_initialized) return; /* no-op */

   if(! type) {
      fprintf(dds_print_fp, "ddsDumpType: NULL TYPE ptr\n");
      return;
   }

   fprintf(dds_print_fp, "TYPE   ");
   ddsDumpSym(indent, (DDS_SYM*)type);

   fprintf(dds_print_fp, "%*sptype= %s; align= %2li; prec= %2li; count= %2li; stroke= %2li\n",
      indent, "", dds_prime[type->ptype].name,
                  (long)type->align,
                  (long)type->prec,
                  (long)type->count,
                  (long)type->stroke);

   fprintf(dds_print_fp, "%*svalue_c= %p; value_a= %p; trail= %p;\n",
      indent, "", (void*)type->value_c,
                  (void*)type->value_a,
                  (void*)type->trail);

   return;
}
