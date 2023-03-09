static char rcsid[]="$Id: stackptr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, return pointer to "index" offset in stack
*** NOTE *** pointer is only valid until next push or pop !!!

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void * (ddsStackPtr)( /* masking macro provided for efficiency */
   DDS_STACK *stack,
   size_t index)
{
   return stack->ptr + index; /* stack "index" converted (char*)  */
}
