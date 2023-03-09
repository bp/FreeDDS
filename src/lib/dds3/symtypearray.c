static char rcsid[]="$Id: symtypearray.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPE array (constant size) in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_TYPE * ddsSymTypeArray(
   DDS_VALUE *value)
{
   DDS_SYM_FLAG flag;
   DDS_TYPE *new_type, *type, *array;
   unsigned long count;
   void *name_ptr;

   type = dds_declare->type;

   if(! type) {
      ddsPanic("ddsSymTypeArr: invalid array type\n");
      type = dds_error_type;
      dds_declare->more = dds_declare->type = type;
   }

   flag = (DDS_SYM_FLAG)(DDS_SYM_ARRAY | type->flag);

   /* assemble a meaningful name for the type */
   name_ptr = (void*)ddsTablePushStr(dds_symbol,
      type->name ? type->name : dds_prime[type->ptype].name);
   name_ptr = (void*)ddsTablePushChar(dds_symbol, name_ptr, '[');
   name_ptr = (void*)ddsTablePushChar(dds_symbol, name_ptr, ']');

   /* initialize and append DDS_TYPE descriptor */
   new_type = PUSH_TYPE(dds_symbol, name_ptr, flag,
      dds_block, dds_declare->more);

   if(value) {
      count = ddsSymConstULong(value, 1, DDS_SYM_CONST_DECL);
   } else {
      count = 0;
      new_type->flag |= DDS_SYM_INCOMPLETE;
   }

   if(type->flag & DDS_SYM_INCOMPLETE) {
      /* example: int y[][][3]; */
      ddsYYError("type of array element is 'incomplete'\n");
      new_type->flag |= DDS_SYM_ERROR;
   }

   new_type->ptype = type->ptype;
   new_type->align = type->align;
   new_type->prec = type->stroke;
   new_type->count = count;
   new_type->stroke = count * type->stroke;

   new_type->value_c = value;

   array = NULL;
   while(type != dds_declare->more) {
      /* scale higher dimensions by new dimension's count */
      type->prec *= count;
      type->stroke *= count;
      /* walk the type chain to the redeclared type */
      assert(type->type && (type->flag & DDS_SYM_ARRAY));
      array = type;
      type = type->type;
   }

   if(array) {
      /* attach this dimension to accumulated (higher) dimensions */
      array->type = new_type;
   } else {
      /* initialize declare type */
      dds_declare->type = new_type;
   }

   return new_type;
}
