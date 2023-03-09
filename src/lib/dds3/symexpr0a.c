static char rcsid[]="$Id: symexpr0a.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_EXPR (no argument) in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

DDS_VALUE * ddsSymExpr0A(
   const char *name,
   DDS_OPCODE opcode)
{
   DDS_SYM_FLAG flag;
   DDS_EXPR *expr;
   DDS_TYPE *type;
   void *value_ptr;

   flag = (DDS_SYM_FLAG)(DDS_SYM_INHIBIT | DDS_SYM_CONST | DDS_SYM_VOLATILE);

   type = dds_prime[HOST_LLONG_DDS].type;

   switch(opcode) {
   case DDS_LINE_TRC_OP:
      if(! (dds_state & DDS_YACC_IO)) {
         ddsYYError("linetrc function not defined in this context\n");
         flag |= DDS_SYM_ERROR;
      }
      assert(HOST_LLONG_P == sizeof(dds_compile->LineTrc));
      value_ptr = &dds_compile->LineTrc;
      break;
   case DDS_REEL_TRC_OP:
      if(! (dds_state & DDS_YACC_IO)) {
         ddsYYError("reeltrc function not defined in this context\n");
         flag |= DDS_SYM_ERROR;
      }
      assert(HOST_LLONG_P == sizeof(dds_compile->ReelTrc));
      value_ptr = &dds_compile->ReelTrc;
      break;
   default:
      ddsPanic("bad opcode\n");
   }

   expr = PUSH_EXPR(dds_symbol, name, flag, dds_block, type);

   expr->ptr = value_ptr;
   expr->opcode = opcode;

   return (DDS_VALUE*)expr;
}
