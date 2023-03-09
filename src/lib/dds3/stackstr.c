static char rcsid[]="$Id: stackstr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, push "string" onto stack (return offset)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

size_t ddsStackStr(
   DDS_STACK *stack,
   const char *str)
{
   size_t size;
   size_t next = stack->top;

   size = strlen(str);

   size++;  /* include terminating NUL */

   stack->top = next + size;
   if(stack->top > stack->max) {
      /* increase allocated stack size */
      if(size > stack->max)
         stack->max += DDS_STACK_MIN * (1 + size / DDS_STACK_MIN);
      else
         stack->max *= 2;
      stack->ptr = (char*)ddsRealloc(stack->ptr, stack->max);
   }

   memcpy(stack->ptr + next, str, size);

   return next; /* "index" (offset) to beginning of string */
}
