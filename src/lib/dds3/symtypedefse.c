static char rcsid[]="$Id: symtypedefse.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPEDEF Struct (End) to symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_OBJECT * ddsSymTypedefSE(
   DDS_BLOCK *block,
   const char *name)
{
   DDS_OBJECT *object;

   ddsSymBlockEnd();
   ddsSymTypeSUE(block);

   dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT;

   name = ddsTablePushStr(dds_symbol, name);
   object = ddsSymDeclare(name);

   DECLARE_CLEAR

   return object;
}
