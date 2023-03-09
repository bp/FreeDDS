static char rcsid[]="$Id: stackset.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, set "index" as top of stack

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

size_t (ddsStackSet)(
   DDS_STACK *stack,
   size_t index)
{
   stack->top = index; /* set top of stack */
   return stack->top; /* new top of stack */
}
