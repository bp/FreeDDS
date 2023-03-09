static char rcsid[]="$Id: binfreeze.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, find object and compile map for read/write/seek processing

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsBinFreeze(
   DDS_BIN *bin)
{
   int status = DDS_OK;

   /* Select the symbol table for the output binary. */
   dds_compile = bin;
   dds_symbol = &bin->symbol;
   dds_block = bin->block_new;

   if(ddsLinkObject(&bin->trace)) status = DDS_BAD;
   cdds_dict(bin->dict_open, "freeze");
   bin->state &= (DDS_BIN_STATE) ~ DDS_BIN_FREEZE;

   return status;
}
