static char rcsid[]="$Id: dumpstack.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_STACK

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpStack(
   int indent,
   DDS_STACK *stack)
{
   if(! dds_initialized) return; /* no-op */

   if(! stack) {
      fprintf(dds_print_fp, "ddsDumpStack: NULL STACK ptr\n");
      return;
   }

   fprintf(dds_print_fp, "STACK &= %p; top= %5li; max= %5li; ptr= %p;\n",
      (void*)stack,
      (long)stack->top,
      (long)stack->max,
      (void*)stack->ptr);

   return;
}
