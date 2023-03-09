static char rcsid[]="$Id: tablepushitem.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create and zero "named" item onto hash table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_ITEM * ddsTablePushItem(
   DDS_TABLE *table,
   const char *name,
   size_t item_size)
{
   DDS_ITEM *new_item, *prev_item;
   size_t bucket;
   DDS_HASH hash;

if(dds_compile && dds_compile->block_old) {
   if(dds_compile->block_old->block)
      ddsPanic("ddsTablePushItem: bad symbol table\n");
}
   new_item = ddsTablePush(table, item_size, HOST_MALLOC_A);

   memset((void*)new_item, '\0', item_size);

   if(name) {
      /* named table item, insert into hash table */
      hash = ddsHash(name);
      bucket = hash % table->size;
      table->count++;
      if(table->chain[bucket]) {
         /* add new item to existing bucket chain */
         for(prev_item = table->chain[bucket];
            prev_item;
            prev_item = prev_item->chain) {
            if(hash == prev_item->hash &&
               ! strcmp(name, prev_item->name)) break; /* non-unique */
         }
         if(! prev_item) table->unique++;
         new_item->chain = table->chain[bucket];
      } else {
         /* new item is initial entry in bucket chain */
         table->load++;
         table->unique++;
      }
      table->chain[bucket] = new_item;
   } else {
      hash = 0;
   }

   new_item->hash = hash;
   new_item->name = name;

   prev_item = table->newest;
   if(prev_item) {
      /* add new item to existing table entries */
      prev_item->newer = new_item;
      new_item->older = prev_item;
      table->newest = new_item;
   } else {
      /* new item is initial entry in hash table */
      table->newest = table->oldest = new_item;
   }

   return new_item;
}
