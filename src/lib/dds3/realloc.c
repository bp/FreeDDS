static char rcsid[]="$Id: realloc.c 229 2011-06-02 18:53:57Z user $";
/*
RCS-t=DDS, realloc memory with error detection and aborting.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void * ddsRealloc(
   void *old,
   size_t size)
{
   void *mem;

#ifdef DDS_HEAP_CHECK
 
   DDS_HEAP *heap;
   size_t padsize = sizeof(char*)*(1+(size-1)/sizeof(char*));
   size_t heap_size = padsize + DDS_HEAP_CREEP + sizeof(DDS_HEAP);
   char *base;
 
   if(! old) {
      dds_mem_use += size;
      mem = ddsMalloc(size);
   } else if((dds_debug_api & DDS_DBG_ALLOC) && !ddsHeapCheck(old)) {
      dds_mem_use += size;
      mem = ddsMalloc(size);
   } else if(size) {
      /* get size increase */
      heap = (DDS_HEAP*)((char*)old - sizeof(DDS_HEAP));
      dds_mem_use += heap_size - (double)heap->size;

      /* reallocate memory */
      mem = (char*)old - DDS_HEAP_CREEP;
      base = realloc(mem, heap_size);
      if(!base) ddsPanic("re-allocate memory, %li bytes, unrecoverable failure\n",
         (long)size);

      /* reset the heap info */
      heap = (DDS_HEAP*)(base + DDS_HEAP_CREEP - sizeof(DDS_HEAP));
      heap->ptr  = heap;
      heap->size = heap_size;
      heap->ptr2 = heap;
      heap->size2= heap_size;
      mem = dds_heap.ptr + heap->idx;
      memcpy(mem, (void*)heap, sizeof(DDS_HEAP));
      mem = base + DDS_HEAP_CREEP + padsize;
      memcpy(mem, (void*)heap, sizeof(DDS_HEAP));
      mem = base + DDS_HEAP_CREEP;
   } else {
      mem = 0;
   }
 
#else /* DDS_HEAP_CHECK */

   if(! old) {
      dds_mem_use += size;
      mem = malloc(size);
   } else if(size) {
      dds_mem_use += size;
      mem = realloc(old, size);
      if(!mem) ddsPanic("re-allocate memory, %li bytes, unrecoverable failure\n",
         (long)size);
   } else {
      mem = 0;
   }

#endif /* DDS_HEAP_CHECK */

   /* Keep tabs on max memory used for logging */
   if (dds_mem_use > dds_mem_max) dds_mem_max = dds_mem_use;

   return mem;
}
