static char rcsid[]="$Id: tableunhash.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, unhash "indexed" hash table entry
   Unlink item from hash table "chain" and overflow "chain".
   Retain item on hash table list "newer" and "older".

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_ITEM * ddsTableUnhash(
   DDS_TABLE *table,
   DDS_ITEM *item)
{
   DDS_ITEM *prev, *test;
   size_t bucket;

   bucket = item->hash % table->size;
   prev = NULL;

   for(test = table->chain[bucket];
      test;
      test = test->chain) {
      if(test == item) {
         /* entry found in symbol table */
         table->count--;
         if(prev) {
            prev->chain = test->chain;
         } else {
            table->chain[bucket] = test->chain;
            if(! test->chain) table->load--;
         }
         test->chain = NULL;
         for(prev = table->chain[bucket];
            prev;
            prev = prev->chain) {
            if(test->hash == prev->hash &&
               ! strcmp(test->name, prev->name)) break; /* non-unique */
         }
         if(! prev) table->unique--;
         break;
      }
      prev = test;
   }
   return test;
}
