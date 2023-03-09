static char rcsid[]="$Id: symtypedefobj.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, append typedef DDS_OBJECT to symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_OBJECT * ddsSymTypedefObj(
   const char *name,
   DDS_PTYPE ptype,
   int align)
{
   DDS_TYPE *type;
   DDS_OBJECT *object;

   name = ddsTablePushStr(dds_symbol, name);

   type = ddsSymTypeCopy(dds_prime[ptype].type);
   if(align && type->align != align) {
      type->flag |= DDS_SYM_ALIGN;
      type->align = align;
   }

   object = PUSH_OBJECT(dds_symbol, name, DDS_SYM_TYPEDEF, dds_block, type);

   return object;
}
