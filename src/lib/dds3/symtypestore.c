static char rcsid[]="$Id: symtypestore.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPE "storage class" in symbol table.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_TYPE * ddsSymTypeStore(
   DDS_SYM_FLAG flag)
{
   DDS_TYPE *type;

   if(dds_block == dds_compile->block_new &&
      (flag & (DDS_SYM_REGISTER | DDS_SYM_AUTO))) {
      /* Example (assume file scope):  auto int j; */
      ddsYYWarn("auto/register storage class not allowed in file scope\n");
   } else if(dds_block->flag &
      (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
      /* Example: struct{auto int j;} x; */
      ddsYYWarn("storage class not allowed in struct/union scope\n");
   }

   if(dds_declare->type) {
      /* use existing type */
      type = dds_declare->type;
      if(dds_declare->flag & DDS_SYM_STORE) {
         /* example: int static extern j; */
         if(! (type->flag & DDS_SYM_ERROR)) {
            dds_declare->flag |= DDS_SYM_ERROR;
            ddsYYError("Only one storage class is allowed\n");
         }
      } else {
         dds_declare->flag |= flag;
      }
   } else {
      /* create and initialize DDS_TYPE descriptor */
      type = ddsSymTypeSpec((DDS_SYM_FLAG)0, (DDS_SYM_FLAG)(DDS_SYM_SPEC));
      dds_declare->flag |= flag;
   }

   return type;
}
