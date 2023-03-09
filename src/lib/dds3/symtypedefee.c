static char rcsid[]="$Id: symtypedefee.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPEDEF enum (end) to symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsSymTypedefEE(
   DDS_BLOCK *block,
   const char *name)
{
   ddsSymTypeSUE(block);
   name = ddsTablePushStr(dds_symbol, name);
   ddsSymDeclare(name);
   DECLARE_CLEAR

   return;
}
