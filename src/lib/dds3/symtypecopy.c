static char rcsid[]="$Id: symtypecopy.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPE via copy in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_TYPE * ddsSymTypeCopy(
   DDS_TYPE *copy_type)
{
   DDS_TYPE *type;

   type = PUSH_TYPE(dds_symbol, copy_type->name, copy_type->flag,
      copy_type->block, copy_type->type);
   type->ptype = copy_type->ptype;
   type->align = copy_type->align;
   type->prec = copy_type->prec;
   type->count = copy_type->count;
   type->stroke = copy_type->stroke;
   type->value_c = copy_type->value_c;
   type->value_a = copy_type->value_a;
   type->trail = copy_type->trail;

   return type;
}
