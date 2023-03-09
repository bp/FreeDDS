static char rcsid[]="$Id: c_name.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, return name of given symbol

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 15 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char* cdds_name(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag)
{
   DDS_BIN *bin;
   DDS_SYM *sym;

   DDS_API_ENTRY("name");

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(NULL))

   SYM_TAG_TO_SYM(sym, bin, sym_tag, DDS_API_EXIT(NULL))

   DDS_API_EXIT(sym->name);
}
