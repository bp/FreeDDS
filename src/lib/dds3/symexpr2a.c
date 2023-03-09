static char rcsid[]="$Id: symexpr2a.c 248 2012-09-05 15:17:42Z user $";
/*
RCS-t=DDS, create DDS_EXPR (two arguments, assignment) in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

DDS_VALUE * ddsSymExpr2A(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,  /* lvalue */
   DDS_VALUE *value2)  /* rvalue */
{
   DDS_SYM_FLAG flag;
   DDS_EXPRA *expra;
   DDS_EXPRC *exprc;
   DDS_EXPR4 *expr, *expr_v1, *expr_v2;
   DDS_PTYPE stype1, stype2;
   DDS_TYPE *type1, *type2;
   DDS_TYPE *type1a, *type2a;
   DDS_TYPE_GENUS genus1a, genus2a;
   DDS_SHAPE shape;
   long count, count2, bias1, bias2;

   assert(opcode == DDS_ASSIGN_OP);

   /****************************************************/
   /* argument usage, iff DDS_ASSIGN_OP retained.      */
   /*    arg1.value.ptr == lvalue == result            */
   /*    arg2.value.ptr == rvalue                      */
   /*    stype1         == left  operand stream type   */
   /*    stype2         == right operand stream type   */
   /*    count          == element count (1 if scalar) */
   /*                                                  */
   /* argument usage, iff morphed into DDS_COPY_OP.    */
   /*    arg1.value.ptr == lvalue == result            */
   /*    arg2.value.ptr == rvalue                      */
   /*    arg3.value.idx == byte count                  */
   /*    lhs_offset     == LHS starting byte offset    */
   /*    rhs_offset     == RHS starting byte offset    */
   /****************************************************/

   type1a = type1 = value1->type;

   type2a = type2 = value2->type;

   flag = (DDS_SYM_FLAG)((value1->flag & (DDS_SYM_ERROR |
      DDS_SYM_RELOC | DDS_SYM_LVALUE | DDS_SYM_ASUE | DDS_SYM_QUAL)) |
      (value2->flag & (DDS_SYM_ERROR | DDS_SYM_DECL)));

   shape = ddsSymTypeShape2(&type1a, &type2a);

   genus1a = dds_prime[type1a->ptype].genus;
   genus2a = dds_prime[type2a->ptype].genus;

   if(! (value1->flag & DDS_SYM_LVALUE)) {
      ddsYYError("left operand not an lvalue, operator '\\=' (assignment)\n");
      flag |= DDS_SYM_ERROR;
   }

   stype1 = dds_prime[type1a->ptype].stype;
   stype2 = dds_prime[type2a->ptype].stype;
   count = 0;

   switch(shape) {
   case DDS_SHAPE_NON:
      ddsYYError("assignment '%s' %s\n",
         dds_opnote[opcode].infix, dds_shape_name[shape]);
      flag |= DDS_SYM_ERROR;
      break;
   case DDS_SHAPE_SS:
      count = 1;
      break;
   case DDS_SHAPE_VV:
      count = type1a->prec ? type1->stroke / type1a->prec : 0;
      count2 = type2a->prec ? type2->stroke / type2a->prec : 0;
      if (count2 < count) count = count2;
      break;

   case DDS_SHAPE_VS:
   case DDS_SHAPE_SV:
      ddsYYError("assignment '%s' %s\n",
         dds_opnote[opcode].infix, dds_shape_name[shape]);
      flag |= DDS_SYM_ERROR;
      break;
   }

   /* inhibit assignment, if vector has zero length. */
   if(! count) flag |= DDS_SYM_INHIBIT;

   if(genus1a == DDS_TYPE_CHARS || genus2a == DDS_TYPE_CHARS) {
      /* non-ANSI restriction, imposed by DDS */
      if(genus1a != DDS_TYPE_CHARS || genus2a != DDS_TYPE_CHARS) {
         ddsYYError("incompatible operand types, operator '\\=' (assignment)\n");
         flag |= DDS_SYM_ERROR;
      }
   } else if((1 << genus1a) & DDS_ARITHMETIC_GENUS) {
      if(((1 << genus1a) & DDS_INTEGRAL_GENUS) &&
         genus2a == DDS_TYPE_PTRS) {
         /* warning, but not an error */
         ddsYYWarn("non-pointer assigned a pointer type\n");
      } else if(genus2a == DDS_TYPE_COMPLEXS) {
         ddsYYError("non-complex assigned a complex, operator '\\=' (assignment)\n");
         flag |= DDS_SYM_ERROR;
      }
   } else if(genus1a == DDS_TYPE_COMPLEXS) {
      if(genus2a != DDS_TYPE_COMPLEXS) {
         ddsYYWarn("complex assigned a non-complex, operator '\\=' (assignment)\n");
         flag |= DDS_SYM_ERROR;
      }
   } else if(genus1a == DDS_TYPE_STRUCTS || genus1a == DDS_TYPE_UNIONS) {
      if(! ddsSymSameType(type1a, type2a)) {
         ddsYYError("incompatible struct/union, operator '\\=' (assignment)\n");
         flag |= DDS_SYM_ERROR;
      }
   } else if(genus1a == DDS_TYPE_PTRS) {
      if(((1 << genus2a) & DDS_ARITHMETIC_PTR_GENUS) &&
         (value2->flag & DDS_SYM_CONST_DECL) &&
         ddsSymNullValue(value2)) {
         /* ok, assign NULL to pointer */
      } else if((1 << genus2a) & DDS_INTEGRAL_GENUS) {
         /* warning, but not an error */
         ddsYYWarn("pointer assigned a non-pointer type\n");
      } else if(genus2a == DDS_TYPE_PTRS) {
         type1a = type1a->type;
         type2a = type2a->type;
         if(! ddsSymSameType(type1a, type2a)) {
            /* warning, but not an error */
            ddsYYWarn("incompatible pointer operand types, operator '\\=' (assignment)\n");
         }
         if(! (type1a->flag & DDS_SYM_CONST) &&
              (type2a->flag & DDS_SYM_CONST)) {
            /* warning, but not an error */
            ddsYYWarn("const to non-const pointer, operator '\\=' (assignment)\n");
         }
      } else {
         ddsYYError("incompatible operand types, operator '\\=' (assignment)\n");
         flag |= DDS_SYM_ERROR;
      }
   } else {
      ddsPanic("can't decode left operand of assignment, prime=%s(%i)\n", 
         dds_prime[type1a->ptype].name, genus1a);
   }

   if(stype1 <= DDS_STYPE_MAX && stype1 >= DDS_STYPE_MIN &&
        stype2 <= DDS_STYPE_MAX && stype2 >= DDS_STYPE_MIN &&
        dds_copy_op[stype1 - DDS_STYPE_MIN][stype2 - DDS_STYPE_MIN]) {
      /* morph from DDS_ASSIGN_OP into DDS_COPY_OP. */
      exprc = PUSH_EXPRC(dds_symbol, name, flag, dds_block, type1);

      ddsSymValuePtr(&exprc->ptr, value1);
      exprc->opcode = DDS_COPY_OP;
      EXPR_ARG(&exprc->arg1, value1);
      EXPR_ARG(&exprc->arg2, value2);
      exprc->arg3.value.idx = type1->stroke;

      bias1 = bias2 = -1;

      if((value1->flag & DDS_SYM_EXPR) &&
         (value2->flag & DDS_SYM_EXPR)) {
         expr_v1 = (DDS_EXPR4*)value1;
         expr_v2 = (DDS_EXPR4*)value2;

         if(expr_v1->opcode == DDS_STRUCT_MEMBER_OP &&
            expr_v2->opcode == DDS_STRUCT_MEMBER_OP) {
            bias1 = expr_v1->arg2.value.idx;
            bias2 = expr_v2->arg2.value.idx;
         }
      }

      exprc->lhs_bias = bias1;
      exprc->rhs_bias = bias2;

      expr = (DDS_EXPR4*)exprc;
   } else {
      /* retain DDS_ASSIGN_OP */
      expra = PUSH_EXPRA(dds_symbol, name, flag, dds_block, type1);

      ddsSymValuePtr(&expra->ptr, value1);
      expra->opcode = opcode;
      EXPR_ARG(&expra->arg1, value1);
      EXPR_ARG(&expra->arg2, value2);
      expra->stype1 = stype1;
      expra->stype2 = stype2;
      expra->count = count;

      expr = (DDS_EXPR4*)expra;
   }

   ddsSymExprDone((DDS_EXPR*)expr);

   return (DDS_VALUE*)expr;
}
