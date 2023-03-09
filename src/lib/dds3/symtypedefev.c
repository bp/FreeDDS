static char rcsid[]="$Id: symtypedefev.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPEDEF enum value in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsSymTypedefEV(
   const char *name,
   int evalue)
{
   DDS_OBJECT *object;
   DDS_VALUE *value;
   DDS_PTYPE ptype = DDS_CTYPE_INT;
   char buf[32];
   int *value_ptr;

   name = ddsTablePushStr(dds_symbol, name);
   object = ddsSymEnumName(name);
   value_ptr = ddsTablePush(dds_symbol, dds_prime[ptype].prec,
      dds_prime[ptype].align);
   *value_ptr = evalue;
   sprintf(buf, "%i", evalue);
   name = ddsTablePushStr(dds_symbol, buf);
   value = ddsSymObjectLit(name, DDS_SYM_CONST_DECL,
      dds_prime[ptype].type, value_ptr, 0);
   ddsSymEnumValue(object, value);

   return;
}
