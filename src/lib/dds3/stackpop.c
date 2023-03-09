static char rcsid[]="$Id: stackpop.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, pop "size" bytes from top of stack

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

size_t (ddsStackPop)(
   DDS_STACK *stack,
   size_t size)
{
   stack->top -= size;/* reduce top */
   return stack->top; /* new top of stack */
}
