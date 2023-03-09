static char rcsid[]="$Id: malloc.c 229 2011-06-02 18:53:57Z user $";
/*
RCS-t=DDS, malloc memory with error detection and aborting.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void * ddsMalloc(
   size_t size)
{
   void *mem;

#ifdef DDS_HEAP_CHECK

   DDS_HEAP *heap;
   size_t padsize = sizeof(char*)*(1+(size-1)/sizeof(char*));
   size_t heap_size = padsize + DDS_HEAP_CREEP + sizeof(DDS_HEAP);
   char *base;

   if(dds_debug_api & DDS_DBG_ALLOC) ddsHeapCheck(NULL);

   if(size) {
      dds_mem_use += heap_size;
      base = malloc(heap_size);
      if(!base) ddsPanic("allocate memory, %li bytes, unrecoverable failure\n",
         (long)size);
      heap = (DDS_HEAP*)(base + DDS_HEAP_CREEP - sizeof(DDS_HEAP));
      heap->ptr = heap;
      heap->size = heap_size;
      heap->idx = dds_heap.top;
      heap->ptr2 = heap;
      heap->size2 = heap_size;
      heap->idx2 = dds_heap.top;
      dds_heap.top += sizeof(DDS_HEAP);
      if(dds_heap.top > dds_heap.max) {
         dds_heap.max *= 2;
         dds_heap.ptr = realloc(dds_heap.ptr, dds_heap.max);
      }
      mem = dds_heap.ptr + heap->idx;
      memcpy(mem, (void*)heap, sizeof(DDS_HEAP));
      mem = base + DDS_HEAP_CREEP + padsize;
      memcpy(mem, (void*)heap, sizeof(DDS_HEAP));
      mem = base + DDS_HEAP_CREEP;
   } else {
      mem = 0;
   }

#else /* DDS_HEAP_CHECK */

   if(size) {
      dds_mem_use += size;
      mem = malloc(size);
      if(!mem) ddsPanic("allocate memory, %li bytes, unrecoverable failure\n",
         (long)size);
   } else {
      mem = 0;
   }

#endif /* DDS_HEAP_CHECK */

   /* Keep tabs on max memory used for logging */
   if (dds_mem_use > dds_mem_max) dds_mem_max = dds_mem_use;

   return mem;
}
