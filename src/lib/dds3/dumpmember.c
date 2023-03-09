static char rcsid[]="$Id: dumpmember.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_MEMBER

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpMember(
   int indent,
   DDS_MEMBER *member)
{
   if(! dds_initialized) return; /* no-op */

   if(! member) {
      fprintf(dds_print_fp, "ddsDumpMember: NULL MEMBER ptr\n");
      return;
   }

   fprintf(dds_print_fp, "MEMBER ");
   ddsDumpSym(indent, (DDS_SYM*)member);

   fprintf(dds_print_fp, "%*soffset= %5li; bias= %2li; value_b= %p;\n",
      indent, "", (long)member->offset,
                  (long)member->adjust,
                  (void*)member->value_b);

   return;
}
