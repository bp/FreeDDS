static char rcsid[]="$Id: symtypequal.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, append DDS_TYPE "qualifier" to symbol table.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_TYPE * ddsSymTypeQual(
   DDS_SYM_FLAG flag,
   DDS_VALUE *value)
{
   DDS_TYPE *type;
   size_t force;

   if(dds_declare->type) {
      /* use existing type */
      type = dds_declare->type;
      if(type->flag & flag) {
         /* example: const const int j; */
         ddsYYWarn("type qualifier specified more than once\n");
      }
      type->flag |= flag;
   } else {
      /* create and initialize DDS_TYPE descriptor */
      type = ddsSymTypeSpec((DDS_SYM_FLAG)0, (DDS_SYM_FLAG)0);
      type->flag |= flag;
   }

   if(flag & DDS_SYM_ALIGN) {
      force = ddsSymConstSizet(value, 0, (DDS_SYM_FLAG)DDS_SYM_CONST_DECL);
      if(force > DDS_ALIGN_MAX) {
         ddsYYError("invalid alignment override for struct/union (%ld > %d)\n",
            (long)force, (int)DDS_ALIGN_MAX);
      } else {
         if(force) type->align = force;
         type->value_a = value;
      }
   }

   return type;
}
