static char rcsid[]="$Id: symtypedefsb.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPEDEF Struct (Begin) to symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_BLOCK * ddsSymTypedefSB(void)
{
   DDS_BLOCK *block;

   ddsSymTypeStore(DDS_SYM_TYPEDEF);
   block = ddsSymBlockBegin(NULL, DDS_SYM_STRUCT, 
      dds_prime[DDS_STYPE_STRUCT].type);

   dds_declare->flag |= DDS_SYM_IDENT;

   return block;
}
