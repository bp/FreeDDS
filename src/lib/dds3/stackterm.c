static char rcsid[]="$Id: stackterm.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, terminate stack descriptor

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsStackTerm(
   DDS_STACK *stack)
{
   if(stack->ptr) ddsFree(stack->ptr);
   stack->top = 0;
   stack->max = 0;
   stack->ptr = NULL;

   return;
}
