static char rcsid[]="$Id: symexpr2b.c 50 2009-02-10 21:29:01Z user $";
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

DDS_VALUE * ddsSymExpr2B(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,
   DDS_VALUE *value2)
{
   DDS_SYM_FLAG flag;
   DDS_PTYPE concur;
   DDS_EXPR3 *expr;
   DDS_EARG arg3;
   DDS_TYPE *type, *type1,  *type2;
   DDS_TYPE *type1a, *type2a;
   DDS_TYPE_GENUS genus1, genus2;
   DDS_SHAPE shape;

   /****************************************************/
   /* argument usage                                   */
   /*    arg1.value.ptr == left_arg                    */
   /*    arg2.value.ptr == right_arg                   */
   /*    arg3.value.idx == element count (1 if scalar) */
   /****************************************************/

   type1a = type1 = value1->type;
   genus1 = dds_prime[type1->ptype].genus;

   type2a = type2 = value2->type;
   genus2 = dds_prime[type2->ptype].genus;

   flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
      ((value1->flag | value2->flag) & (DDS_SYM_ERROR | DDS_SYM_ARRAY)) |
      ((value1->flag & value2->flag) & (DDS_SYM_DECL)));

   shape = ddsSymTypeShape2(&type1a, &type2a);

   arg3.value.idx = 1;
   arg3.track = NULL;

   switch(opcode) {
   case DDS_POW_OP:
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid left operand type, function 'pow' (power)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid right operand type, function 'pow' (power)\n");
         flag |= DDS_SYM_ERROR;
      }
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("function 'pow' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_ATAN2_OP:
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid left operand type, function 'atan2' (arctangent2)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid right operand type, function 'atan2' (arctangent2)\n");
         flag |= DDS_SYM_ERROR;
      }
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("function 'atan2' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_MULTIPLY_OP_INT:
      if(! ((1 << genus1) & DDS_COMPLEX_GENUS)) {
         ddsYYError("invalid left operand type, operator '*' (multiply)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus2) & DDS_COMPLEX_GENUS)) {
         ddsYYError("invalid right operand type, operator '*' (multiply)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_MULTIPLY_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_MULTIPLY_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_MULTIPLY_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_MULTIPLY_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_MULTIPLY_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_MULTIPLY_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_MULTIPLY_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_MULTIPLY_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_MULTIPLY_OP_LDOUBLE; break;
      case DDS_CTYPE_CPX:     opcode = DDS_MULTIPLY_OP_CPX;     break;
      case DDS_CTYPE_DBLCPX:  opcode = DDS_MULTIPLY_OP_DBLCPX;  break;
      case DDS_CTYPE_LDBLCPX: opcode = DDS_MULTIPLY_OP_LDBLCPX; break;
      }
      type = dds_prime[concur].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_DIVIDE_OP_INT:
      if(! ((1 << genus1) & DDS_COMPLEX_GENUS)) {
         ddsYYError("invalid left operand type, operator '/' (divide)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus2) & DDS_COMPLEX_GENUS)) {
         ddsYYError("invalid right operand type, operator '/' (divide)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_DIVIDE_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_DIVIDE_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_DIVIDE_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_DIVIDE_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_DIVIDE_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_DIVIDE_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_DIVIDE_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_DIVIDE_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_DIVIDE_OP_LDOUBLE; break;
      case DDS_CTYPE_CPX:     opcode = DDS_DIVIDE_OP_CPX;     break;
      case DDS_CTYPE_DBLCPX:  opcode = DDS_DIVIDE_OP_DBLCPX;  break;
      case DDS_CTYPE_LDBLCPX: opcode = DDS_DIVIDE_OP_LDBLCPX; break;
      }
      type = dds_prime[concur].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_MODULO_OP_INT:
      if(! ((1 << genus1) & DDS_INTEGRAL_GENUS)) {
         ddsYYError("invalid left operand type, operator '%' (modulo)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus2) & DDS_INTEGRAL_GENUS)) {
         ddsYYError("invalid right operand type, operator '%' (modulo)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_MODULO_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_MODULO_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_MODULO_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_MODULO_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_MODULO_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_MODULO_OP_ULLONG;  break;
      }
      type = dds_prime[concur].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_PLUS_OP_INT:
      if(! ((1 << genus1) & DDS_COMPLEX_GENUS) ||
         ! ((1 << genus2) & DDS_COMPLEX_GENUS)) {
         ddsYYError("invalid operand types, operator '+' (addition)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_PLUS_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_PLUS_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_PLUS_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_PLUS_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_PLUS_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_PLUS_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_PLUS_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_PLUS_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_PLUS_OP_LDOUBLE; break;
      case DDS_CTYPE_CPX:     opcode = DDS_PLUS_OP_CPX;     break;
      case DDS_CTYPE_DBLCPX:  opcode = DDS_PLUS_OP_DBLCPX;  break;
      case DDS_CTYPE_LDBLCPX: opcode = DDS_PLUS_OP_LDBLCPX; break;
      }
      type = dds_prime[concur].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_MINUS_OP_INT:
      if(! ((1 << genus1) & DDS_COMPLEX_GENUS) ||
         ! ((1 << genus2) & DDS_COMPLEX_GENUS)) {
         ddsYYError("invalid operand types, operator '-' (subtraction)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_MINUS_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_MINUS_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_MINUS_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_MINUS_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_MINUS_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_MINUS_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_MINUS_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_MINUS_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_MINUS_OP_LDOUBLE; break;
      case DDS_CTYPE_CPX:     opcode = DDS_MINUS_OP_CPX;     break;
      case DDS_CTYPE_DBLCPX:  opcode = DDS_MINUS_OP_DBLCPX;  break;
      case DDS_CTYPE_LDBLCPX: opcode = DDS_MINUS_OP_LDBLCPX; break;
      }
      type = dds_prime[concur].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_LEFT_SHIFT_OP_INT:
   case DDS_RIGHT_SHIFT_OP_INT:
      ddsYYError("ddsSymExpr2B: '<<' and '>>' support deferred\n");
      ddsPanic("sorry\n");
      break;
   case DDS_LESS_THAN_OP_INT:
      type = dds_prime[DDS_CTYPE_INT].type;
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS) ||
         ! ((1 << genus2) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand types, operator '<' (less than)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_LESS_THAN_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_LESS_THAN_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_LESS_THAN_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_LESS_THAN_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_LESS_THAN_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_LESS_THAN_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_LESS_THAN_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_LESS_THAN_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_LESS_THAN_OP_LDOUBLE; break;
      }
      value1 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_GREATER_THAN_OP_INT:
      type = dds_prime[DDS_CTYPE_INT].type;
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS) ||
         ! ((1 << genus2) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand types, operator '>' (greater than)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_GREATER_THAN_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_GREATER_THAN_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_GREATER_THAN_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_GREATER_THAN_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_GREATER_THAN_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_GREATER_THAN_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_GREATER_THAN_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_GREATER_THAN_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_GREATER_THAN_OP_LDOUBLE; break;
      }
      value1 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_LESS_EQUAL_OP_INT:
      type = dds_prime[DDS_CTYPE_INT].type;
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS) ||
         ! ((1 << genus2) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand types, operator '<=' (less than or equal to)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_LESS_EQUAL_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_LESS_EQUAL_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_LESS_EQUAL_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_LESS_EQUAL_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_LESS_EQUAL_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_LESS_EQUAL_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_LESS_EQUAL_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_LESS_EQUAL_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_LESS_EQUAL_OP_LDOUBLE; break;
      }
      value1 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_GREATER_EQUAL_OP_INT:
      type = dds_prime[DDS_CTYPE_INT].type;
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS) ||
         ! ((1 << genus2) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand types, operator '>=' (greater than or equal to)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_GREATER_EQUAL_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_GREATER_EQUAL_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_GREATER_EQUAL_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_GREATER_EQUAL_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_GREATER_EQUAL_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_GREATER_EQUAL_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_GREATER_EQUAL_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_GREATER_EQUAL_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_GREATER_EQUAL_OP_LDOUBLE; break;
      }
      value1 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_EQUAL_TO_OP_INT:
      type = dds_prime[DDS_CTYPE_INT].type;
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS) ||
         ! ((1 << genus2) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand types, operator '==' (equal to)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_EQUAL_TO_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_EQUAL_TO_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_EQUAL_TO_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_EQUAL_TO_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_EQUAL_TO_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_EQUAL_TO_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_EQUAL_TO_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_EQUAL_TO_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_EQUAL_TO_OP_LDOUBLE; break;
      }
      value1 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_NOT_EQUAL_TO_OP_INT:
      type = dds_prime[DDS_CTYPE_INT].type;
      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS) ||
         ! ((1 << genus2) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand types, operator '!=' (not equal to)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_NOT_EQUAL_TO_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_NOT_EQUAL_TO_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_NOT_EQUAL_TO_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_NOT_EQUAL_TO_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_NOT_EQUAL_TO_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_NOT_EQUAL_TO_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_NOT_EQUAL_TO_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_NOT_EQUAL_TO_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_NOT_EQUAL_TO_OP_LDOUBLE; break;
      }
      value1 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE,
         dds_prime[concur].type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_BIT_AND_OP_INT:
      if(! ((1 << genus1) & DDS_INTEGRAL_GENUS)) {
         ddsYYError("invalid left operand type, operator '&' (bitwise AND)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus2) & DDS_INTEGRAL_GENUS)) {
         ddsYYError("invalid right operand type, operator '&' (bitwise AND)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_BIT_AND_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_BIT_AND_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_BIT_AND_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_BIT_AND_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_BIT_AND_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_BIT_AND_OP_ULLONG;  break;
      }
      type = dds_prime[concur].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
         type = dds_error_type;
      }
      break;
   case DDS_BIT_XOR_OP_INT:
      if(! ((1 << genus1) & DDS_INTEGRAL_GENUS)) {
         ddsYYError("invalid left operand type, operator '^' (bitwise XOR)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus2) & DDS_INTEGRAL_GENUS)) {
         ddsYYError("invalid right operand type, operator '^' (bitwise XOR)\n")
;
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_BIT_XOR_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_BIT_XOR_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_BIT_XOR_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_BIT_XOR_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_BIT_XOR_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_BIT_XOR_OP_ULLONG;  break;
      }
      type = dds_prime[concur].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
         type = dds_error_type;
      }
      break;
   case DDS_BIT_OR_OP_INT:
      if(! ((1 << genus1) & DDS_INTEGRAL_GENUS)) {
         ddsYYError("invalid left operand type, operator '|' (bitwise OR)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus2) & DDS_INTEGRAL_GENUS)) {
         ddsYYError("invalid right operand type, operator '|' (bitwise OR)\n")
;
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1a->ptype][type2a->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_BIT_OR_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_BIT_OR_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_BIT_OR_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_BIT_OR_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_BIT_OR_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_BIT_OR_OP_ULLONG;  break;
      }
      type = dds_prime[concur].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
         type = dds_error_type;
      }
      break;
   case DDS_LOGICAL_AND_OP:
      type = dds_prime[DDS_CTYPE_INT].type;
      if(! ((1 << genus1) & DDS_COMPLEX_OR_PTR_GENUS)) {
         ddsYYError("invalid left operand type, operator '&&' (logical AND)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus2) & DDS_COMPLEX_OR_PTR_GENUS)) {
         ddsYYError("invalid right operand type, operator '&&' (logical AND)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_LOGICAL_OR_OP:
      type = dds_prime[DDS_CTYPE_INT].type;
      if(! ((1 << genus1) & DDS_COMPLEX_OR_PTR_GENUS)) {
         ddsYYError("invalid left operand type, operator '||' (logical OR)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(! ((1 << genus2) & DDS_COMPLEX_OR_PTR_GENUS)) {
         ddsYYError("invalid right operand type, operator '||' (logical OR)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_SS) {
         ddsYYError("multiply '*' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   default:
      ddsPanic("bad opcode\n");
   }

   expr = PUSH_EXPR3(dds_symbol, name, flag,
      dds_block, type);

   expr->ptr = ddsTablePush(dds_symbol, type->stroke, type->align);
   expr->opcode = opcode;
   EXPR_ARG(&expr->arg1, value1);
   EXPR_ARG(&expr->arg2, value2);
   expr->arg3 = arg3;

   ddsSymExprDone((DDS_EXPR*)expr);

   return (DDS_VALUE*)expr;
}
