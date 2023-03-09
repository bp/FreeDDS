static char rcsid[]="$Id: symtypedefsm.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPEDEF Struct Member in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

void ddsSymTypedefSM(
   const char *name,
   DDS_OBJECT *object,
   long count)
{
   /* object must have typedef storage class */
   DDS_VALUE *expr;
   DDS_TYPE *type;
   DDS_OBJECT *object1;
   char nam[10];
   const char *name_ptr;
   int	*value_ptr;

   /* clear declare type if trail differs from old */

   type = dds_declare->more;
   if (!type || (object != type->trail)) {
      DECLARE_CLEAR;
   } else {
      DECLARE_MORE;
   }

   type = ddsSymTypeSpec(dds_prime[object->type->ptype].type->flag,
      (DDS_SYM_FLAG)0);
   type->name   = object->type->name;
   type->ptype  = object->type->ptype;
   type->align  = object->type->align;
   type->prec   = object->type->prec;
   type->count  = object->type->count;
   type->stroke = object->type->stroke;
   type->trail  = object;

   dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT;

   name = ddsTablePushStr(dds_symbol, name);

   if (count) {
      if (count > 999999999) {
         ddsError("ddsSymTypedefSM: count=%i, too large!", count);
         count = 999999999;
      }

      /* create argument object count */
      value_ptr = ddsTablePush(dds_symbol, 
         dds_prime[DDS_CTYPE_INT].prec, dds_prime[DDS_CTYPE_INT].align);

      if (count > 0) {
         *value_ptr = count;
         sprintf(nam, "%li", count);
      } else {
         *value_ptr = 1;
         sprintf(nam, "1");
      }

      name_ptr = ddsTablePushStr(dds_symbol, nam); 

      object1 = (DDS_OBJECT*)ddsSymObjectLit(name_ptr, 
         (DDS_SYM_FLAG)DDS_SYM_PREP, dds_prime[DDS_CTYPE_INT].type,  
         value_ptr, 0);

      /* create member type */
      if (count > 0) {
         ddsSymTypeArray((DDS_VALUE*)object1);
      } else {
         expr = ddsSymExpr1A(NULL, DDS_AXIS_SIZE_OP, 
            (DDS_VALUE*)object1);
         expr->flag |= DDS_SYM_INHIBIT;
         ddsSymTypeArray(expr);
      }
   }

   ddsSymMember(name);
}
