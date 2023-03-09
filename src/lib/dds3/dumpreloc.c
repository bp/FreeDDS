static char rcsid[]="$Id: dumpreloc.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_RELOC

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpReloc(
   int indent,
   DDS_VALUE *value)
{
   DDS_RELOC *reloc, *reloc_stop;
   DDS_STACK *stack;
   int i;

   if(! dds_initialized) return; /* no-op */

   if(! value) {
      fprintf(dds_print_fp, "ddsDumpReloc: NULL VALUE ptr\n");
      return;
   }

   if(! value->reloc) return; /* no dependencies */

   /* stack contains an array of DDS_RELOC descriptors */
   stack = value->reloc;
   reloc = (DDS_RELOC*)stack->ptr;
   reloc_stop = (DDS_RELOC*)(stack->ptr + stack->top);

   /* first reloc entry is deviant (meta parameters). */
   fprintf(dds_print_fp, "%*sRELOC last= %p;",
      indent, "", (void*)reloc->ptrptr);

   /* Subsequent reloc entries describe pointers that reference value. */
   /* Each pointer must be changed to (ptr + offset). */
   /* The fixed offset is non-zero when referencing a struct member. */
   i = 0;
   for(reloc++; reloc != reloc_stop; reloc++) {
      if(! ((i + 4) % 8)) fprintf(dds_print_fp, "\n%*s", indent, "");
      fprintf(dds_print_fp, "  %p+%lu",
         (void*)reloc->ptrptr, (unsigned long)reloc->offset);
      if(i++ > 1000) fprintf(dds_print_fp, "  QUITE");
   }

   fprintf(dds_print_fp, "\n");

   return;
}
