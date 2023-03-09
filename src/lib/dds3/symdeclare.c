static char rcsid[]="$Id: symdeclare.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_OBJECT for declared type in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_OBJECT * ddsSymDeclare(
   const char *name)
{
   DDS_TYPE *type;
   DDS_SYM_FLAG flag;
   DDS_OBJECT *object;
   void *value_ptr;
   size_t stroke;

   assert(dds_declare->type && "ddsSymDeclare: dds declaration logic error");

#ifdef BAD_IDEA
   if(dds_declare->flag & DDS_SYM_TYPEDEF) {
      type = ddsSymTypeCopy(dds_declare->type);
      type->name = name;
   } else {
      type = dds_declare->type;
   }
#else
   type = dds_declare->type;
#endif

   flag = (DDS_SYM_FLAG)((dds_declare->flag & DDS_SYM_STORE) |
      (type->flag &
         (DDS_SYM_ERROR | DDS_SYM_ASUE | DDS_SYM_QUAL | DDS_SYM_INCOMPLETE)));

   if(type->flag & DDS_SYM_SUE) {
      /* needed by uncompile */
      type->name = name;
   }

   /* lex context, want existing object (not IDENTIFIER_TOKEN) */
   /* Example: int foo[enum_name]; */
   dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT;

   object = PUSH_OBJECT(dds_symbol, name, flag,
      dds_block, type);

   if(type->flag & DDS_SYM_INCOMPLETE) {
      object->flag |= DDS_SYM_ERROR;
      ddsYYError("type specification is 'incomplete'\n");
      value_ptr = NULL;
   } else if(type->flag & DDS_SYM_ERROR) {
      object->flag |= DDS_SYM_ERROR;
      ddsYYError("error in previous type specification\n");
      value_ptr = NULL;
   } else if(flag & DDS_SYM_TYPEDEF) {
      value_ptr = NULL;
   } else if(type->flag &
      (DDS_SYM_CONST_PREP | DDS_SYM_CONST_DECL | DDS_SYM_CONST_INIT)) {
      /* optimize ?, merge all enumerated symbols ? other constants ? */
      stroke = type->stroke;
      value_ptr = ddsTablePush(dds_symbol, stroke, type->align);
   } else {
      /* translation unit scope */
      stroke = type->stroke;
      value_ptr = ddsTablePush(dds_symbol, stroke, type->align);
      memset(value_ptr, '\0', stroke);
   }

   object->ptr = value_ptr;

   return object;
}
