static char rcsid[]="$Id: symtypecast.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create "cast" DDS_EXPR in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

DDS_VALUE * ddsSymTypeCast(
   DDS_SYM_FLAG flag,
   DDS_TYPE *type,
   DDS_VALUE *value1)
{
   DDS_SYM_FLAG save_flag, object_flag;
   DDS_EXPR *expr;
   DDS_OBJECT *object;
   DDS_TYPE *type1;
   DDS_BOOLEAN ignore = DDS_FALSE;
   DDS_OPCODE action;

   assert(flag == DDS_SYM_CAST || flag == DDS_SYM_COERCE);

   type1 = value1->type;

   action = DDS_ASSIGN_OP;  /* default */

   switch(dds_prime[type->ptype].stype) {
   case DDS_STYPE_ASCII1:
      switch(dds_prime[type1->ptype].stype) {
      case DDS_STYPE_ASCII1:
         action = DDS_NO_OP;
         break;
      case DDS_STYPE_EBCDIC1:
      case DDS_STYPE_INTEGER1:
      case DDS_STYPE_UNSIGNED1:
         ignore = DDS_TRUE;
         break;
      }
      break;
   case DDS_STYPE_EBCDIC1:
      switch(dds_prime[type1->ptype].stype) {
      case DDS_STYPE_EBCDIC1:
         action = DDS_NO_OP;
         break;
      case DDS_STYPE_ASCII1:
      case DDS_STYPE_INTEGER1:
      case DDS_STYPE_UNSIGNED1:
         ignore = DDS_TRUE;
         break;
      }
      break;
   case DDS_STYPE_INTEGER1:
      switch(dds_prime[type1->ptype].stype) {
      case DDS_STYPE_INTEGER1:
         action = DDS_NO_OP;
         break;
      case DDS_STYPE_ASCII1:
      case DDS_STYPE_EBCDIC1:
      case DDS_STYPE_UNSIGNED1:
         ignore = DDS_TRUE;
         break;
      }
      break;
   case DDS_STYPE_UNSIGNED1:
      switch(dds_prime[type1->ptype].stype) {
      case DDS_STYPE_UNSIGNED1:
         action = DDS_NO_OP;
         break;
      case DDS_STYPE_ASCII1:
      case DDS_STYPE_EBCDIC1:
      case DDS_STYPE_INTEGER1:
         ignore = DDS_TRUE;
         break;
      }
      break;
   case DDS_STYPE_INTEGER2:
      if(dds_prime[type1->ptype].stype == DDS_STYPE_INTEGER2)
         action = DDS_NO_OP;
      else if(dds_prime[type1->ptype].stype == DDS_STYPE_UNSIGNED2)
         ignore = DDS_TRUE;
      break;
   case DDS_STYPE_UNSIGNED2:
      if(dds_prime[type1->ptype].stype == DDS_STYPE_UNSIGNED2)
         action = DDS_NO_OP;
      else if(dds_prime[type1->ptype].stype == DDS_STYPE_INTEGER2)
         ignore = DDS_TRUE;
      break;
   case DDS_STYPE_INTEGER4:
      if(dds_prime[type1->ptype].stype == DDS_STYPE_INTEGER4)
         action = DDS_NO_OP;
      else if(dds_prime[type1->ptype].stype == DDS_STYPE_UNSIGNED4)
         ignore = DDS_TRUE;
      break;
   case DDS_STYPE_UNSIGNED4:
      if(dds_prime[type1->ptype].stype == DDS_STYPE_UNSIGNED4)
         action = DDS_NO_OP;
      else if(dds_prime[type1->ptype].stype == DDS_STYPE_INTEGER4)
         ignore = DDS_TRUE;
      break;
   case DDS_STYPE_INTEGER8:
      if(dds_prime[type1->ptype].stype == DDS_STYPE_INTEGER8)
         action = DDS_NO_OP;
      else if(dds_prime[type1->ptype].stype == DDS_STYPE_UNSIGNED8)
         ignore = DDS_TRUE;
      break;
   case DDS_STYPE_UNSIGNED8:
      if(dds_prime[type1->ptype].stype == DDS_STYPE_UNSIGNED8)
         action = DDS_NO_OP;
      else if(dds_prime[type1->ptype].stype == DDS_STYPE_INTEGER8)
         ignore = DDS_TRUE;
      break;
   default:
      if(dds_prime[type->ptype].stype ==
         dds_prime[type1->ptype].stype) {
         action = DDS_NO_OP;
      }
      break;
   }

   if(action == DDS_NO_OP) {
      /* identical binary representation (original and cast types) */
      if(flag & DDS_SYM_COERCE) {
         /* nothing to coerce */
         return value1;
      }
      if((type->flag  & DDS_SYM_CONST) == (type1->flag & DDS_SYM_CONST) &&
          type->align == type1->align) {
         /* CONST flags and alignment are also equivalent. */
         /* return, iff redundant casts should be eliminated. */
         return value1;
      }
      ignore = DDS_TRUE;
   }

   flag |= (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (type->flag   & (DDS_SYM_ERROR | DDS_SYM_ASUE | DDS_SYM_QUAL)) |
         (value1->flag & (DDS_SYM_ERROR | DDS_SYM_DECL)));

   if(ignore) {
      /* cast/coerce logical "type" only change, */
      /* i.e. the bit pattern is identical for the two types. */
      save_flag = value1->flag;
      value1->flag = (DDS_SYM_FLAG)(DDS_SYM_LVALUE |
         (value1->flag & ~ DDS_SYM_CONST));
      expr = (DDS_EXPR*)ddsSymExpr2A(NULL, DDS_ASSIGN_OP,
         value1, value1);
      value1->flag = save_flag;
      expr->type = type;
      flag |= DDS_SYM_INHIBIT;
   } else {
      object_flag = (DDS_SYM_FLAG)(DDS_SYM_LVALUE |
         (type->flag & (DDS_SYM_ERROR | DDS_SYM_ASUE)) |
         (value1->flag & DDS_SYM_DECL));
      object = PUSH_OBJECT(dds_symbol, value1->name, object_flag,
         dds_block, type);
      object->ptr = ddsTablePush(dds_symbol, type->stroke, type->align);
      expr = (DDS_EXPR*)ddsSymExpr2A(NULL, DDS_ASSIGN_OP,
         (DDS_VALUE*)object, value1);
   }

   expr->flag |= (DDS_SYM_FLAG)(flag | DDS_SYM_CONST);

   return (DDS_VALUE*)expr;
}
