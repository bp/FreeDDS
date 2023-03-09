static char rcsid[]="$Id: tablelookup.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, lookup "named" hash table entry

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_ITEM * ddsTableLookup(
   DDS_TABLE *table,
   const char *name)
{
   DDS_ITEM *item;
   size_t bucket;
   DDS_HASH hash;

   if(table->load &&
      ((1.0 > table->alpha &&
          table->alpha < (double)table->load / (double)table->size) ||
       (1.0 < table->alpha &&
          table->alpha < (double)table->unique / (double)table->load))) {
      /* increase size of table and rehash named entries */
      ddsTableRehash((DDS_TABLE*)table);
   }

   hash = ddsHash(name);
   bucket = hash % table->size;

   for(item = table->chain[bucket]; item; item = item->chain) {
      if(hash == item->hash && ! strcmp(name, item->name)) break;
   }
   return item;
}
