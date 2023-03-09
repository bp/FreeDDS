static char rcsid[]="$Id: c_memuse.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, check memory usage thru heap

*/

/**********************************************/
/***   Copyright (C) 2006                   ***/
/***   BP America, Inc, Houston, TX         ***/
/***   Jerry Ehlers, 10 Oct 2006            ***/
/**********************************************/

#include "dds_sys.h"

size_t cdds_memuse()
{
   DDS_HEAP *heap, *heap_stop;
   size_t size;

   DDS_API_ENTRY("memuse")


   heap_stop = (DDS_HEAP*)(dds_heap.ptr + dds_heap.top);

   size = 0;
   for(heap = (DDS_HEAP*)dds_heap.ptr;
      heap != heap_stop;
      heap++) {

      if(heap != (DDS_HEAP*)(dds_heap.ptr + heap->idx)) {
         /* immediate panic */
         ddsPanic("heap validation stack is corrupted\n");
      }

      size += heap->size;
   }

   DDS_API_EXIT(size)
}
