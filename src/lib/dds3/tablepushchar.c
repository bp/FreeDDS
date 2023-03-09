static char rcsid[]="$Id: tablepushchar.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, push one char for contigous array onto table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * ddsTablePushChar(
   DDS_TABLE *table,
   const char *start,
   char value)
{
   char *new_start;
   size_t new_size;

   if(start) {
      /* append new char to existing array */
      if(table->page.top == table->page.max) {
         /* new char overflows current page */
         new_size = table->page.max - (start - table->page.ptr);
         table->page.top -= new_size;
         new_start = ddsTablePush(table, 2 * new_size, 1);
         table->page.top -= new_size;
         memcpy(new_start, start, new_size);
         start = new_start;
      }
   } else {
      /* first char of a new array */
      if(table->page.top == table->page.max) {
         /* new char overflows current page */
         ddsTablePush(table, 1, 1);
         table->page.ptr--;
      }
      start = table->page.ptr;
   }

   table->page.ptr[table->page.top++] = value;
   return start;
}
