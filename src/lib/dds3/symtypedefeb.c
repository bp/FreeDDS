static char rcsid[]="$Id: symtypedefeb.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPEDEF enum (begin) to symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_BLOCK * ddsSymTypedefEB(
   DDS_PTYPE ptype)
{
   DDS_BLOCK *block;

   DECLARE_CLEAR;
   ddsSymTypeStore(DDS_SYM_TYPEDEF);
   block = ddsSymBlockBegin(NULL, DDS_SYM_ENUM, dds_prime[ptype].type);
   ddsSymBlockEnd();
   dds_declare->flag |= DDS_SYM_IDENT;

   return block;
}
