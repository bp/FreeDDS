static char rcsid[]="$Id: symexpr2c.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, append DDS_EXPR (struct/union member ref) to symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

DDS_VALUE * ddsSymExpr2C(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,
   const char *name2)
{
   DDS_SYM_FLAG flag;
   DDS_MEMBER *member2;
   DDS_EXPR4 *expr;
   DDS_EARG arg2, arg3, arg4;
   DDS_TYPE *type, *type1, *type1a;
   DDS_TYPE_GENUS genus1, genus1a;
   DDS_SHAPE shape;
   int n;
   char *cptr;
   void *value_ptr = NULL;
   long bias;

   /****************************************************/
   /* argument usage                                   */
   /*    arg1.value.ptr == left_arg                    */
   /*    arg2.value.ptr == bias (cummulative offset)   */
   /*    arg3.value.idx == element count (1 if scalar) */
   /*    arg4.value.idx == bytes per element           */
   /****************************************************/

   type1a = type1 = value1->type;
   genus1 = dds_prime[type1->ptype].genus;

   arg2.track = NULL;
   arg2.value.idx = 0;
   arg3.track = NULL;
   arg3.value.idx = 0;
   arg4.track = NULL;
   arg4.value.idx = 0;

   flag = (DDS_SYM_FLAG)(DDS_SYM_INHIBIT |
      (value1->flag & (DDS_SYM_ERROR |
         DDS_SYM_RELOC | DDS_SYM_QUAL | DDS_SYM_DECL)));

   switch(opcode) {
   case DDS_STRUCT_MEMBER_OP:
      shape = ddsSymTypeShape(&type1a);
      genus1a = dds_prime[type1a->ptype].genus;
      if(! ((1 << genus1a) & DDS_STRUCT_OR_UNION_GENUS)) {
         ddsYYError("invalid left operand type, operator '.' (struct/union member)\n");
         flag |= DDS_SYM_ERROR;
         type = dds_error_type;
      } else if(type1->flag & DDS_SYM_INCOMPLETE) {
         ddsYYError("incomplete left operand member, operator '.' (struct/union member)\n");
         flag |= DDS_SYM_ERROR;
         type = dds_error_type;
      } else {
         /* Check if this is an indexed field */
         /* If so, save the index separated from the field */
         n = -1;
         cptr = strrchr(name2, '[');
         if (cptr) {
            sscanf(cptr, "[%d]", &n);
            cptr[0] = '\0';
         }

         member2 = ddsSymLookupMem(dds_symbol, name2, type1a->block, &bias);
         if(member2) {
            arg2.value.idx = bias;
            type = member2->type;
            flag |= (DDS_SYM_FLAG)(member2->flag & (DDS_SYM_ERROR |
               DDS_SYM_RELOC | DDS_SYM_ASUE | DDS_SYM_QUAL));
            /* "the result is an lvalue if the first expression is an */
            /* lvalue, and if the type of the second expression is not */
            /* an array" (K&R 2nd Ed, A7.3.3) */
            /* DDS SUPPORTS ARRAY OPERATORS. */
            flag |= (DDS_SYM_FLAG)(value1->flag & DDS_SYM_LVALUE);
            if (n >= 0) {
               /* setup for an indexed field */
               cptr[0] = '[';
               arg2.value.idx = bias + n * type->prec;
               arg3.value.idx = 1;
               arg4.value.idx = type->prec;
               type->count = 1;
               type->stroke = type->prec;
               type->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_ARRAY;
               flag &= (DDS_SYM_FLAG) ~ DDS_SYM_ARRAY;
               break;
            }
         } else {
            ddsYYError("'%s' is not a member of struct/union\n", name2);
            flag |= DDS_SYM_ERROR;
            type = dds_error_type;
         }
      }
      if(shape == DDS_SHAPE_S) {
         arg3.value.idx = 1;
      } else {
         flag &= (DDS_SYM_FLAG) ~ (DDS_SYM_VOLATILE | DDS_SYM_ALIGN);
         arg3.value.idx = type1a->prec ? type1->stroke / type1a->prec : 0;
         value_ptr = ddsTablePush(dds_symbol,
            arg3.value.idx * type->prec, type->align);
      }
      arg4.value.idx = type->prec;
      break;
   case DDS_STRUCT_POINTER_OP:
      shape = ddsSymTypeShape(&type1a);
      genus1a = dds_prime[type1a->ptype].genus;
      if(genus1 == DDS_TYPE_PTRS) {
         if(! ((1 << genus1a) & DDS_STRUCT_OR_UNION_GENUS)) {
            ddsYYError("invalid left operand pointie type, operator '->' (struct/union pointer)\n");
            flag |= DDS_SYM_ERROR;
            type = dds_error_type;
         } else if(type1a->flag & DDS_SYM_INCOMPLETE) {
            ddsYYError("incomplete left operand struct/union, operator '->' (struct/union pointer)\n");
            flag |= DDS_SYM_ERROR;
            type = dds_error_type;
         } else {
            member2 = ddsSymLookupMem(dds_symbol, name2, type1a->block, &bias);
            if(member2) {
               arg2.value.idx = bias;
               type = member2->type;
               flag |= (DDS_SYM_FLAG)(member2->flag & (DDS_SYM_ERROR |
                  DDS_SYM_RELOC | DDS_SYM_ASUE | DDS_SYM_QUAL));
               /* "the result is an lvalue if the type is not an array" */
               /* (K&R 2nd Ed, A7.3.3) */
               /* ALWAYS AN LVALUE BECAUSE DDS SUPPORTS ARRAY OPERATORS */
               flag |= DDS_SYM_LVALUE;
            } else {
               ddsYYError("'%s' is not a member of struct/union\n", name2);
               flag |= DDS_SYM_ERROR;
               type = dds_error_type;
            }
         }
         if(shape == DDS_SHAPE_S) {
            arg3.value.idx = 1;
         } else {
            flag &= (DDS_SYM_FLAG) ~ (DDS_SYM_VOLATILE | DDS_SYM_ALIGN);
            arg3.value.idx = type1a->prec ? type1->stroke / type1a->prec : 0;
            value_ptr = ddsTablePush(dds_symbol,
               arg3.value.idx * type->prec, type->align);
         }
         arg4.value.idx = type->prec;
      } else {
         ddsYYError("invalid left operand type, operator '->' (struct/union pointer)\n");
      }
      break;
   default:
      ddsPanic("bad opcode\n");
   }

   expr = PUSH_EXPR4(dds_symbol, name, flag, dds_block, type);

   expr->ptr = value_ptr;
   expr->opcode = opcode;
   EXPR_ARG(&expr->arg1, value1);
   expr->arg2 = arg2;
   expr->arg3 = arg3;
   expr->arg4 = arg4;

   return (DDS_VALUE*)expr;
}
