static char rcsid[]="$Id: symexpr1a2.c 16 2009-01-06 21:52:47Z user $";
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

DDS_VALUE * ddsSymExpr1A2(
   const char *name)
{
   DDS_SYM_FLAG flag;
   DDS_EXPR *expr;
   DDS_TYPE *type;
   int *value_ptr;
#if defined(_WIN32) || defined(MINGW)
   size_t bias;
#else
   long bias;
#endif

   flag = (DDS_SYM_FLAG)(DDS_SYM_CONST | DDS_SYM_DECL | DDS_SYM_INHIBIT);

   type = dds_prime[DDS_CTYPE_INT].type;

   value_ptr = ddsTablePush(dds_symbol, type->stroke, type->align);

   if(dds_map_value) {
      /* Compiling an rvalue for a map expression. */
      if(ddsSymLookupMem(dds_symbol, name,
         dds_map_value->type->block, &bias)) {
         /* TRUE, name is a member of struct/union */
         *value_ptr = 1;
      } else {
         /* FALSE, name is not a member of struct/union */
         *value_ptr = 0;
      }
   } else {
      ddsYYError("'exist' not defined in this context\n");
      flag |= DDS_SYM_ERROR;
      *value_ptr = 0;
   }

   expr = PUSH_EXPR(dds_symbol, name, flag, dds_block, type);

   expr->ptr = value_ptr;
   expr->opcode = DDS_EXIST_OP;

   return (DDS_VALUE*)expr;
}
