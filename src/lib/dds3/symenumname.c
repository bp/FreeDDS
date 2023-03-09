static char rcsid[]="$Id: symenumname.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create enumerated DDS_OBJECT (implicit value) in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_OBJECT * ddsSymEnumName(
   const char *name)
{
   DDS_SYM_FLAG flag;
   DDS_OBJECT *object;
   DDS_TYPE *type;
   DDS_BOOLEAN redefined = DDS_FALSE;

   if(UNIQUE_OBJECT(dds_symbol, name, dds_block)) {
      ddsYYError("enum identifier '%s' multiply defined\n", name);
      redefined = DDS_TRUE;
   }

   type = dds_declare->type;
   flag = (DDS_SYM_FLAG)(DDS_SYM_STATIC | DDS_SYM_ENUM |
      DDS_SYM_CONST | DDS_SYM_DECL);

   object = PUSH_OBJECT(dds_symbol, name, flag,
      dds_block, type);

   /* lex context, want existing object (not IDENTIFIER_TOKEN) */
   /* Example: enum {name = existing_enum_name;} foo; */
   dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT;

   if(redefined) {
      object->flag |= (DDS_SYM_FLAG)(DDS_SYM_ERROR | DDS_SYM_MULTIPLE);
      object->hash = ~ object->hash; /* name will never match */
   }

   return object;
}
