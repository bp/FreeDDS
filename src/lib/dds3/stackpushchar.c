static char rcsid[]="$Id: stackpushchar.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, push one character onto stack (return offset to char)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

size_t (ddsStackPushChar)(
   DDS_STACK *stack,
   int c)
{
   if(stack->top >= stack->max) {
      stack->max *= 2; /* double stack size */
      stack->ptr = (char*)ddsRealloc(stack->ptr, stack->max);
   }
   stack->ptr[stack->top] = c;

   return stack->top++;
}
