static char rcsid[]="$Id: stackinit.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, initialize stack descriptor

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

size_t ddsStackInit(
   DDS_STACK *stack,
   size_t min)
{
   stack->max = DDS_STACK_MIN * (1 + min / DDS_STACK_MIN);
   stack->ptr = (char*)ddsMalloc(stack->max);
   stack->top = 0;

   return stack->max; /* actual initial stack size */
}
