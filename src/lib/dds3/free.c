static char rcsid[]="$Id: free.c 229 2011-06-02 18:53:57Z user $";
/*
RCS-t=DDS, free memory with NULL pointer check.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsFree(
   void *mem)
{
#ifdef DDS_HEAP_CHECK
   DDS_HEAP *heap, *heap_stack;

   if((dds_debug_api & DDS_DBG_FREE) && !ddsHeapCheck(mem)) return;

   if(mem) {
      heap = (DDS_HEAP*)((char*)mem - sizeof(DDS_HEAP));
      dds_mem_use -= heap->size;
      heap_stack = (DDS_HEAP*)(dds_heap.ptr + heap->idx);
      heap_stack->size = 0;
      free((void*)((char*)mem - DDS_HEAP_CREEP));
   }

#else /* DDS_HEAP_CHECK */

   if(mem) free(mem);

#endif /* DDS_HEAP_CHECK */

   return;
}

