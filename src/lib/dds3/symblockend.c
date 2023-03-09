static char rcsid[]="$Id: symblockend.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, exit current local DDS_BLOCK in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsSymBlockEnd(void)
{
   DDS_SYM *sym;

   /* unlink local symbols (remove from global older/newer list) */
   /* relink local symbols (blk->first & last, older/newer list) */
   dds_block->first = (DDS_SYM*)dds_block->newer;
   dds_block->last = dds_block->newer ? (DDS_SYM*)dds_symbol->newest : NULL;
   dds_block->newer = NULL;
   dds_symbol->newest = (DDS_ITEM*)dds_block;
   if(dds_block->first) {
      sym = (DDS_SYM*)dds_block->first;
      sym->older = NULL;
   }

   /* exit local block */
   dds_block = dds_block->block;

   DECLARE_OUTER;

   return;
}
