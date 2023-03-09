static char rcsid[]="$Id: c_parent.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, return symbol tag of parent block

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 11 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

SYM_TAG (cdds_parent)(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
   DDS_BIN *bin;
   DDS_SYM *sym;
   SYM_TAG new_tag;

   DDS_API_ENTRY("parent")

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_SYM(sym, bin, sym_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TO_SYM_TAG(new_tag, &bin->sym_tag, sym->block)

   DDS_API_EXIT(new_tag)
}
