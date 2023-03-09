static char rcsid[]="$Id: stackpushptr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, push one pointer onto stack

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const void ** (ddsStackPushPtr)(
   DDS_STACK *stack,
   const void *ptr)
{
   const void **ptrptr;
   size_t next = stack->top;

   stack->top += sizeof(void*);
   if(stack->top > stack->max) {
      /* increase allocated stack size */
      stack->max *= 2;
      stack->ptr = (char*)ddsRealloc(stack->ptr, stack->max);
   }
   
   ptrptr = (const void**)(stack->ptr + next);
   *ptrptr = ptr;

   return ptrptr;
}
