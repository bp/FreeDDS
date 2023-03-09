static char rcsid[]="$Id: heapcheck.c 148 2010-04-22 17:22:32Z user $";
/*
RCS-t=DDS, validate dds heap blocks

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsHeapCheck(
   void *ptr)
{
   static DDS_BOOLEAN recurse = DDS_FALSE;
   DDS_HEAP *heap, *heap_stop, *head, *tail;
   DDS_BOOLEAN bad_head, bad_tail;

   if(recurse) {
      ddsError("ddsHeapCheck: recursion\n");
      return DDS_FALSE;
   } else {
      recurse = DDS_TRUE;
   }

   heap_stop = (DDS_HEAP*)(dds_heap.ptr + dds_heap.top);

   for(heap = (DDS_HEAP*)dds_heap.ptr; heap != heap_stop; heap++) {

      if(heap != (DDS_HEAP*)(dds_heap.ptr + heap->idx)) {
         /* immediate panic */
         ddsPanic("heap validation stack is corrupted\n");
      }

      if(! heap->size) {
         /* already freed */
         continue;
      }

      /* force any system heap errors as early as possible */
      head = (DDS_HEAP*)malloc((heap->size < DDS_MB) ? heap->size : DDS_MB);
      if(head) {
         free((char*)head);
      } else {
         ddsPanic("heapcheck malloc test failed\n");
      }
      
      head = heap->ptr;
      tail = (DDS_HEAP*)((char*)heap->ptr - DDS_HEAP_CREEP + heap->size);
         

      if(memcmp((void*)heap, (void*)head, sizeof(DDS_HEAP)))
         bad_head = DDS_TRUE;
      else
         bad_head = DDS_FALSE;

      if(memcmp((void*)heap, (void*)tail, sizeof(DDS_HEAP)))
         bad_tail = DDS_TRUE;
      else
         bad_tail = DDS_FALSE;

      if(bad_head && bad_tail) {
         ddsVerbose("corrupted heap head and tail, block= %p\n",
            (void*)((char*)head + sizeof(DDS_HEAP)));
         ddsDebug("heap=%x %x %i %i %i %i\n\thead=%x %x %i %i %i %i\n\ttail=%x %x %i %i %i %i\n", 
            heap->ptr, heap->ptr2, heap->idx, heap->idx2, heap->size, 
            heap->size2, head->ptr, head->ptr2, head->idx, head->idx2,  
            head->size, head->size2, tail->ptr, tail->ptr2, tail->idx,  
            tail->idx2, tail->size, tail->size2);
      } else if(bad_head) {
         ddsVerbose("corrupted heap head, block= %p\n",
            (void*)((char*)head + sizeof(DDS_HEAP)));
         ddsDebug("heap=%x %x %i %i %i %i\n\thead=%x %x %i %i %i %i\n", 
            heap->ptr, heap->ptr2, heap->idx, heap->idx2, heap->size, 
            heap->size2, head->ptr, head->ptr2, head->idx, head->idx2,  
            head->size, head->size2);
      } else if(bad_tail) {
         ddsVerbose("corrupted heap tail, block= %p\n",
            (void*)((char*)head + sizeof(DDS_HEAP)));
         ddsDebug("heap=%x %x %i %i %i %i\n\ttail=%x %x %i %i %i %i\n", 
            heap->ptr, heap->ptr2, heap->idx, heap->idx2, heap->size, 
            heap->size2, tail->ptr, tail->ptr2, tail->idx, tail->idx2,  
            tail->size, tail->size2);
      }
   }

   if(ptr) {
      heap = (DDS_HEAP*)((char*)ptr - sizeof(DDS_HEAP));
      head = (DDS_HEAP*)(dds_heap.ptr + heap->idx);
      if(heap->idx >= dds_heap.top) {
         ddsVerbose("corrupted heap beyond heap table, block= %p\n", 
            ptr);
         bad_head = DDS_TRUE;
      } else if(memcmp((void*)heap, (void*)head, sizeof(DDS_HEAP))) {
         ddsVerbose("corrupted heap, block= %p\n", ptr);
         bad_head = DDS_TRUE;
         ddsDebug("heap=%x %x %i %i %i %i\n\thead=%x %x %i %i %i %i\n", 
            heap->ptr, heap->ptr2, heap->idx, heap->idx2, heap->size, 
            heap->size2, head->ptr, head->ptr2, head->idx, head->idx2,  
            head->size, head->size2);
      }
   }

   recurse = DDS_FALSE;

   if(bad_head || bad_tail) {
      ddsError("heap is corrupted\n");
      return(DDS_FALSE);
   }

   return(DDS_TRUE);
}
