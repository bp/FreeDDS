static char rcsid[]="$Id: symobjectlit.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_OBJECT literal in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_VALUE * ddsSymObjectLit(
   const char *name,
   DDS_SYM_FLAG flag,
   DDS_TYPE *copy_type,
   void *value_ptr,
   size_t count)
{
   DDS_OBJECT *object;
   DDS_TYPE *type, *atype;

   type = ddsSymTypeCopy(copy_type);
   type->flag |= DDS_SYM_CONST;

   if(count) {
      atype = type;
      type = ddsSymTypeCopy(atype);
      type->flag |= DDS_SYM_ARRAY;
      type->count = count;
      type->stroke = count * copy_type->prec;
      type->type = atype;
   }

   flag |= (DDS_SYM_FLAG)(DDS_SYM_INHIBIT | DDS_SYM_CONST |
      (type->flag & DDS_SYM_ARRAY));

   object = PUSH_OBJECT(dds_symbol, name,
      flag, dds_block, type);

   object->ptr = value_ptr;

   return (DDS_VALUE*)object;
}
