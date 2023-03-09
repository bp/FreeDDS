static char rcsid[]="$Id: symexpr1a3.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_EXPR (one argument, unary ops) in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

DDS_VALUE * ddsSymExpr1A3(
   const char *name,
   DDS_OPCODE opcode,
   DDS_TYPE *type1)
{
   DDS_SYM_FLAG flag;
   DDS_EXPR1 *expr;
   DDS_TYPE *type;
   void *value_ptr = NULL;

   switch(opcode) {
   case DDS_SIZEOF_TYPE_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST | DDS_SYM_DECL | DDS_SYM_INHIBIT |
         (type1->flag & DDS_SYM_ERROR));
#if defined(_WIN32) || defined(MINGW)
      type = dds_prime[HOST_LLONG_DDS].type;
      assert(HOST_LLONG_P == sizeof(type1->stroke));
#else
      type = dds_prime[HOST_LONG_DDS].type;
      assert(HOST_LONG_P == sizeof(type1->stroke));
#endif
      if(type1->flag & (DDS_SYM_FUNC | DDS_SYM_FIELD | DDS_SYM_INCOMPLETE)) {
         ddsYYError("size not defined, operator 'sizeof' (type_name)\n");
         flag |= DDS_SYM_ERROR;
         value_ptr = dds_error_value->ptr;
      } else {
         value_ptr = &type1->stroke;
      }
      break;
   default:
      ddsPanic("bad opcode\n");
   }

   expr = PUSH_EXPR1(dds_symbol, name, flag, dds_block, type);

   expr->ptr = value_ptr;
   expr->opcode = opcode;
   expr->arg1.value.ptr = NULL;
   expr->arg1.track = (DDS_VALUE*)type1; /* deviant */

   ddsSymExprDone((DDS_EXPR*)expr);

   return (DDS_VALUE*)expr;
}
