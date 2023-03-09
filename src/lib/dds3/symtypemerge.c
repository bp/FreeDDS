static char rcsid[]="$Id: symtypemerge.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, merge declaration and existing type into symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_TYPE * ddsSymTypeMerge(
   DDS_TYPE *merge)
{
   DDS_TYPE *type;

   /* lex context, want IDENTIFIER_TOKEN (not existing object) */
   /* Required for declarators after type specifiers, i.e. */
   /*    int new_identifier; */
   dds_declare->flag |= DDS_SYM_IDENT;

   if(dds_declare->type) {
      /* use existing type */
      type = dds_declare->type;
      if((type->flag & DDS_SYM_SPEC) || type->type) {
         /* example: int float j; */
         /* example: struct{int j;} integer2 x; */
         /* example: int * integer2 j; */
         type->flag |= DDS_SYM_ERROR;
         ddsYYError("invalid combination of type specifiers\n");
         return type;
      }
   } else {
      /* create and initialize DDS_TYPE descriptor */
      type = PUSH_TYPE(dds_symbol, NULL, (DDS_SYM_FLAG)0,
         dds_block, NULL);
      dds_declare->more = dds_declare->type = type;
   }

   type->name = merge->name;

   type->flag |= (DDS_SYM_FLAG)(merge->flag &
      (DDS_SYM_ERROR | DDS_SYM_SPEC | DDS_SYM_INCOMPLETE));

   type->type = merge->type;
   type->ptype = merge->ptype;
   if(! type->value_a) {
      /* 7/9/98 rls, only merge alignment if not explicitly defined. */
      type->align = merge->align;
   }
   type->prec = merge->prec;
   type->count = merge->count;
   type->stroke = merge->stroke;
   if(merge->flag & DDS_SYM_SUE) {
      /* deviant usage of block pointer */
      type->block = merge->block;
   } else {
      type->block = dds_block;
   }


   return type;
}
