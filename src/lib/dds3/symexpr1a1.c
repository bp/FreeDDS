static char rcsid[]="$Id: symexpr1a1.c 16 2009-01-06 21:52:47Z user $";
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

DDS_VALUE * ddsSymExpr1A1(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1)
{
   DDS_SYM_FLAG flag;
   DDS_EXPR1 *expr;
   DDS_TYPE_GENUS genus1;

   genus1 = dds_prime[value1->type->ptype].genus;

   flag = (DDS_SYM_FLAG)(DDS_SYM_CONST | DDS_SYM_INHIBIT |
      (value1->flag & (DDS_SYM_ERROR | DDS_SYM_RELOC | DDS_SYM_ARRAY |
         DDS_SYM_QUAL | DDS_SYM_DECL)));

   if(! ((1 << genus1) & DDS_COMPLEX_GENUS)) {
      ddsYYError("invalid operand type, operator '+' (unary plus)\n");
      flag |= DDS_SYM_ERROR;
   }

   expr = PUSH_EXPR1(dds_symbol, name, flag, dds_block, value1->type);

   ddsSymValuePtr(&expr->ptr, value1);
   expr->opcode = opcode;
   EXPR_ARG(&expr->arg1, value1);

   return (DDS_VALUE*)expr;
}
