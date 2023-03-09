static char rcsid[]="$Id: tablepush.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, push "size" bytes ("align" offset) onto table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void * ddsTablePush(
   DDS_TABLE *table,
   size_t size,
   size_t align)
{
   DDS_PAGE save, *page;
   size_t max, total, top, next;

   top = next = table->page.top;
   if(next % align) next += align - (next % align);

   table->page.top = next + size;
   if(table->page.top > table->page.max) {
      /* page overflow, allocate another one */
      table->page.top = top;
      save = table->page;
      /* total storage for all existing pages */
      total = table->page.max;
      for(page = (DDS_PAGE*)table->page.ptr;
         page;
         page = (DDS_PAGE*)page->ptr) {
         total += page->max;
      }

      next = sizeof(DDS_PAGE);
      if(next % align) next += align - (next % align);

      max = next + size;
      table->page.top = max;
      if(max > total)
         table->page.max = DDS_STACK_MIN * (1 + max / DDS_STACK_MIN);
      else
         table->page.max = total; /* double page storage */
      table->page.ptr = ddsMalloc(table->page.max);
      *(DDS_PAGE*)table->page.ptr = save;
   }

   return table->page.ptr + next;
}
