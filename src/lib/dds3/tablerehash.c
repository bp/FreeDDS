static char rcsid[]="$Id: tablerehash.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, rehash "named" hash table entries

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

size_t ddsTableRehash(
   DDS_TABLE *table)
{
   DDS_ITEM *item, *prev_item, *next_item, **old_chain, **last_item;
   size_t bucket, new_size, old_size, old_bucket; 

   /* increase hash table size to improve efficiency */
   new_size = ddsPrimeUp(table->size * (1 + (1 / table->beta)));

   if(new_size <= table->size) new_size = ddsPrimeUp(table->size +1);
   old_size = table->size;
   old_chain = table->chain;
   table->size = new_size;
   table->chain = (DDS_ITEM**)ddsZalloc(new_size * sizeof(DDS_ITEM*));
   last_item    = (DDS_ITEM**)ddsZalloc(new_size * sizeof(DDS_ITEM*));

   table->load = 0;
   table->unique = 0;

   for(old_bucket = 0; old_bucket < old_size; old_bucket++) {
      for(item = old_chain[old_bucket]; item; item = next_item) {
         next_item = item->chain;
         item->chain = NULL;
         if(! item->name) continue; /* deleted ? */
         bucket = item->hash % new_size;
         if(table->chain[bucket]) {
            /* add item to existing bucket chain */
            for(prev_item = table->chain[bucket];
               prev_item;
               prev_item = prev_item->chain) {
               if(item->hash == prev_item->hash &&
                  ! strcmp(item->name, prev_item->name)) break; /* non-unique */
            }
            if(! prev_item) table->unique++;
            last_item[bucket]->chain = item;
         } else {
            /* item is initial entry in bucket chain */
            table->load++;
            table->unique++;
            table->chain[bucket] = item;
         }
         last_item[bucket] = item;
      }
   }

   ddsFree((void*)old_chain);
   ddsFree((void*)last_item);

   return table->size;
}
