static char rcsid[]="$Id: tableinit.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, initialize hash table descriptor

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

size_t ddsTableInit(
   DDS_TABLE *table,
   size_t size,
   size_t min,
   double alpha,
   double beta)
{
   table->size = ddsPrimeUp(size);
   table->chain = (DDS_ITEM**)ddsZalloc(table->size * sizeof(DDS_ITEM*));
   table->newest = NULL;
   table->oldest = NULL;
   table->load = 0;
   table->count = 0;
   table->unique = 0;
   table->alpha = alpha;
   table->beta = beta;

   table->page.top = sizeof(DDS_PAGE);
   table->page.max = DDS_PAGE_MIN *
      (1 + (min + sizeof(DDS_PAGE)) / DDS_STACK_MIN);
   table->page.ptr = ddsMalloc(table->page.max);
   memset(table->page.ptr, '\0', sizeof(DDS_PAGE));

   return table->size;
}
