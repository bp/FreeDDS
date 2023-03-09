static char rcsid[]="$Id: tableterm.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, terminate "named" hash table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsTableTerm(
   DDS_TABLE *table)
{
   DDS_PAGE *page, *prev;

   if(table->chain) ddsFree(table->chain);
   table->chain = NULL;
   table->oldest = NULL;
   table->newest = NULL;
   table->size = 0;
   table->load = 0;
   table->count = 0;
   table->unique = 0;
   table->alpha = 0;
   table->beta = 0;

   /* free all pages used for symbols */
   for(page = (DDS_PAGE*)table->page.ptr; page; page = prev) {
      prev = (DDS_PAGE*)page->ptr;
      ddsFree(page);
   }

   table->page.top = 0;
   table->page.max = 0;
   table->page.ptr = NULL;

   return;
}
