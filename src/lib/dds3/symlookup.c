static char rcsid[]="$Id: symlookup.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, lookup name in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_SYM * ddsSymLookup(
   const DDS_TABLE *table,
   const char *name,
   DDS_SYM_FLAG sym_flag,
   DDS_BLOCK *first_block,
   DDS_BLOCK *last_block)
{
   DDS_SYM *chain_sym;
   DDS_BLOCK *test_block;
   size_t bucket;
   DDS_HASH hash;

   assert(sym_flag == DDS_SYM_BLOCK || sym_flag == DDS_SYM_MEMBER ||
      sym_flag == DDS_SYM_OBJECT);

   if(table->load &&
      ((1.0 > table->alpha &&
          table->alpha < (double)table->load / (double)table->size) ||
       (1.0 < table->alpha &&
          table->alpha < (double)table->unique / (double)table->load))) {
      /* increase size of table and rehash named entries */
      ddsTableRehash((DDS_TABLE*)table);
   }

   /******************************************************************/
   /* If first_block equals last_block, then only one block          */
   /* is searched (used to detect multiple declarations in a block). */
   /* If last_block equals block_new, then blocks are searched       */
   /* from the first_block through enclosing blocks (nested scopes). */
   /******************************************************************/

   hash = ddsHash(name);
   bucket = hash % table->size;

   for(test_block = first_block;
      test_block;
      test_block = test_block->block) {
      for(chain_sym = (DDS_SYM*)table->chain[bucket];
         chain_sym;
         chain_sym = (DDS_SYM*)chain_sym->chain) {
         if(chain_sym->block == test_block &&
            chain_sym->hash == hash &&
            (chain_sym->flag & sym_flag) &&
            ! strcmp(chain_sym->name, name)) {
            return chain_sym;
         }
      }
      if(test_block == last_block) break;
   }
   return NULL;
}
