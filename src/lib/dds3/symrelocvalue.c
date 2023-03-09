static char rcsid[]="$Id: symrelocvalue.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, evaluate DDS_RELOC entry in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsSymRelocValue(
   DDS_VALUE *value)
{
   DDS_RELOC *reloc, *reloc_stop;
   DDS_STACK *stack;
   char *ptr = value->ptr;

   /* stack contains an array of DDS_RELOC descriptors */
   stack = value->reloc;
   reloc = (DDS_RELOC*)stack->ptr;
   reloc_stop = (DDS_RELOC*)(stack->ptr + stack->top);

   /* first reloc entry is deviant (meta parameters). */
   reloc->ptrptr = value->ptr;

   /* Subsequent reloc entries describe pointers that reference value. */
   /* Each pointer must be changed to (ptr + offset). */
   /* The fixed offset is non-zero when referencing a struct member. */
   for(reloc++; reloc != reloc_stop; reloc++) {
      *reloc->ptrptr = ptr + reloc->offset;
   }

   return;
}
