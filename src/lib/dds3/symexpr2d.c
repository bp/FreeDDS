static char rcsid[]="$Id: symexpr2d.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_EXPR (two arguments) in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

DDS_VALUE * ddsSymExpr2D(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,
   DDS_VALUE *value2)
{
   DDS_SYM_FLAG flag;
   DDS_EXPR4 *expr;
   DDS_EARG arg3, arg4;
   DDS_TYPE *type, *type1,  *type2;
   DDS_TYPE *type1a, *type2a;
   DDS_TYPE_GENUS genus1, genus2;
   DDS_SHAPE shape;

   /****************************************************/
   /* argument usage                                   */
   /*    arg1.value.ptr == pointer or subscript        */
   /*    arg2.value.ptr == subscript or pointer        */
   /*    arg3.value.idx == element count (1 if scalar) */
   /*    arg4.value.idx == bytes per element           */
   /****************************************************/

   type1a = type1 = value1->type;
   genus1 = dds_prime[type1->ptype].genus;

   type2a = type2 = value2->type;
   genus2 = dds_prime[type2->ptype].genus;

   shape = ddsSymTypeShape2(&type1a, &type2a);

   arg3.value.idx = 0;
   arg3.track = NULL;
   arg4.value.idx = 0;
   arg4.track = NULL;

   if((value1->flag & DDS_SYM_ERROR) ||
      (value2->flag & DDS_SYM_ERROR)) {
      ddsYYError("bad operand types, operator '[]' (array reference)\n");
      flag = DDS_SYM_ERROR;
      type = type1;
   } else if(genus1 == DDS_CTYPE_PTR || (type1->flag & DDS_SYM_ARRAY)) {
      type = type1->type;
      arg3.value.idx = type->count;
      arg4.value.idx = type->prec;
      flag = (DDS_SYM_FLAG)(DDS_SYM_RELOC |
         (value1->flag & DDS_SYM_QUAL) |
         (type->flag & DDS_SYM_ASUE) |
         ((value1->flag | value2->flag) & DDS_SYM_ERROR) |
         ((value1->flag & value2->flag) & DDS_SYM_DECL));
      /* "It is an lvalue if the operand is a pointer of arithmetic, */
      /* structure, union, or pointer type." */
      /* (K&R 2nd Ed, A7.3.3) */
      if((1 << type->ptype) & DDS_COMPLEX_STRUCT_UNION_PTR_GENUS) {
         flag |= DDS_SYM_LVALUE;
      }
      if((1 << genus2) & DDS_INTEGRAL_GENUS) {
         opcode = DDS_ARRAY_REF_OP_PTR_SUB;
         value2 = ddsSymTypeCast(DDS_SYM_COERCE,
            dds_prime[HOST_SIZE_T_DDS].type, value2);
      } else {
         ddsYYError("invalid subscript type, operator '[]' (array reference)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_SS && shape != DDS_SHAPE_VS) {
         ddsYYError("vector[non-scalar] not allowed\n");
         flag |= DDS_SYM_ERROR;
      }
   } else if(genus2 == DDS_CTYPE_PTR || (type2->flag & DDS_SYM_ARRAY)) {
      type = type2->type;
      arg3.value.idx = type->count;
      arg4.value.idx = type->prec;
      flag = (DDS_SYM_FLAG)(DDS_SYM_RELOC |
         (value2->flag & DDS_SYM_QUAL) |
         (type->flag & DDS_SYM_ASUE) |
         ((value2->flag | value1->flag) & DDS_SYM_ERROR) |
         ((value2->flag & value1->flag) & DDS_SYM_DECL));
      /* "It is an lvalue if the operand is a pointer of arithmetic, */
      /* structure, union, or pointer type." */
      /* (K&R 2nd Ed, A7.3.3) */
      if((1 << type->ptype) & DDS_COMPLEX_STRUCT_UNION_PTR_GENUS) {
         flag |= DDS_SYM_LVALUE;
      }
      if((1 << genus1) & DDS_INTEGRAL_GENUS) {
         opcode = DDS_ARRAY_REF_OP_SUB_PTR;
         value1 = ddsSymTypeCast(DDS_SYM_COERCE,
            dds_prime[HOST_SIZE_T_DDS].type, value1);
      } else {
         ddsYYError("invalid subscript type, operator '[]' (array reference)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_SS && shape != DDS_SHAPE_SV) {
         ddsYYError("vector[non-scalar] not allowed\n");
         flag |= DDS_SYM_ERROR;
      }
   } else {
      ddsYYError("invalid operand types, operator '[]' (array reference)\n");
      flag = DDS_SYM_ERROR;
      type = type1;
   }

   expr = PUSH_EXPR4(dds_symbol, name, flag, dds_block, type);

   expr->opcode = opcode;
   EXPR_ARG(&expr->arg1, value1);
   EXPR_ARG(&expr->arg2, value2);
   expr->arg3 = arg3;
   expr->arg4 = arg4;

   ddsSymExprDone((DDS_EXPR*)expr);

   return (DDS_VALUE*)expr;
}
