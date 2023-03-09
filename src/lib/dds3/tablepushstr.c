static char rcsid[]="$Id: tablepushstr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, push string onto table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * ddsTablePushStr(
   DDS_TABLE *table,
   const char *str)
{
   DDS_PAGE save, *page;
   size_t size, max, total, next;

   next = table->page.top;
   size = 1 + strlen(str);

   table->page.top = next + size;
   if(table->page.top > table->page.max) {
      /* page overflow, allocate another one */
      table->page.top = next;
      save = table->page;
      /* total storage for all existing pages */
      total = table->page.max;
      for(page = (DDS_PAGE*)table->page.ptr;
         page;
         page = (DDS_PAGE*)page->ptr) {
         total += page->max;
      }

      next = sizeof(DDS_PAGE);
      table->page.top = max = next + size;
      if(max > total)
         table->page.max = DDS_STACK_MIN * (1 + max / DDS_STACK_MIN);
      else
         table->page.max = total; /* double page storage */
      table->page.ptr = ddsMalloc(table->page.max);
      *(DDS_PAGE*)table->page.ptr = save;
   }

   return memcpy((void*)(table->page.ptr + next), str, size);
}
