static char rcsid[]="$Id: symexpr1a.c 177 2010-08-20 15:57:35Z user $";
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

DDS_VALUE * ddsSymExpr1A(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1)
{
   DDS_SYM_FLAG flag;
   DDS_PTYPE concur;
   DDS_EXPR1 *expr;
   DDS_TYPE *type, *type1, *type1a;
   DDS_TYPE_GENUS genus1;
   DDS_SHAPE shape;
   int axis;
   void *value_ptr = NULL;

   type1a = type1 = value1->type;
   genus1 = dds_prime[type1->ptype].genus;

   shape = ddsSymTypeShape(&type1a);

   flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
      (value1->flag & (DDS_SYM_ERROR | DDS_SYM_ARRAY | DDS_SYM_DECL)));

   switch(opcode) {
   case DDS_UNARY_PLUS_OP:
      return ddsSymExpr1A1(name, opcode, value1);
   case DDS_UNARY_MINUS_OP_INT:
      if(! ((1 << genus1) & DDS_COMPLEX_GENUS)) {
         ddsYYError("invalid operand type, operator '-' (unary minus)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1->ptype][type1->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_UNARY_MINUS_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_UNARY_MINUS_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_UNARY_MINUS_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_UNARY_MINUS_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_UNARY_MINUS_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_UNARY_MINUS_OP_ULLONG;  break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_UNARY_MINUS_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_UNARY_MINUS_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_UNARY_MINUS_OP_LDOUBLE; break;
      case DDS_CTYPE_CPX:     opcode = DDS_UNARY_MINUS_OP_CPX;     break;
      case DDS_CTYPE_DBLCPX:  opcode = DDS_UNARY_MINUS_OP_DBLCPX;  break;
      case DDS_CTYPE_LDBLCPX: opcode = DDS_UNARY_MINUS_OP_LDBLCPX; break;
      }
      type = dds_prime[concur].type;
      if(shape != DDS_SHAPE_S) {
         ddsYYError("unary '-' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_COMPLEMENT_OP_INT:
      if(! ((1 << genus1) & DDS_COMPLEX_GENUS)) {
         ddsYYError("invalid operand type, operator '~' (one's complement)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1->ptype][type1->ptype];
      switch(concur) {
      case DDS_CTYPE_INT:     opcode = DDS_COMPLEMENT_OP_INT;     break;
      case DDS_CTYPE_LONG:    opcode = DDS_COMPLEMENT_OP_LONG;    break;
      case DDS_CTYPE_LLONG:   opcode = DDS_COMPLEMENT_OP_LLONG;   break;
      case DDS_CTYPE_UINT:    opcode = DDS_COMPLEMENT_OP_UINT;    break;
      case DDS_CTYPE_ULONG:   opcode = DDS_COMPLEMENT_OP_ULONG;   break;
      case DDS_CTYPE_ULLONG:  opcode = DDS_COMPLEMENT_OP_ULLONG;  break;
      }
      type = dds_prime[concur].type;
      if(shape != DDS_SHAPE_S) {
         ddsYYError("unary '~' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_NEGATION_OP_INT:
      if(! ((1 << genus1) & DDS_COMPLEX_GENUS)) {
         ddsYYError("invalid operand type, operator '!' (logical negation)\n");
         flag |= DDS_SYM_ERROR;
      }
      concur = dds_concur[type1->ptype][type1->ptype];
      switch(concur) {
      case DDS_CTYPE_SCHAR:
      case DDS_CTYPE_UCHAR:
      case DDS_CTYPE_CHAR:    opcode = DDS_NEGATION_OP_CHAR;    break;
      case DDS_CTYPE_USHORT:
      case DDS_CTYPE_SHORT:   opcode = DDS_NEGATION_OP_SHORT;   break;
      case DDS_CTYPE_UINT:
      case DDS_CTYPE_INT:     opcode = DDS_NEGATION_OP_INT;     break;
      case DDS_CTYPE_ULONG:
      case DDS_CTYPE_LONG:    opcode = DDS_NEGATION_OP_LONG;    break;
      case DDS_CTYPE_ULLONG:
      case DDS_CTYPE_LLONG:   opcode = DDS_NEGATION_OP_LLONG;   break;
      case DDS_CTYPE_FLOAT:   opcode = DDS_NEGATION_OP_FLOAT;   break;
      case DDS_CTYPE_DOUBLE:  opcode = DDS_NEGATION_OP_DOUBLE;  break;
      case DDS_CTYPE_LDOUBLE: opcode = DDS_NEGATION_OP_LDOUBLE; break;
      case DDS_CTYPE_CPX:     opcode = DDS_NEGATION_OP_CPX;     break;
      case DDS_CTYPE_DBLCPX:  opcode = DDS_NEGATION_OP_DBLCPX;  break;
      case DDS_CTYPE_LDBLCPX: opcode = DDS_NEGATION_OP_LDBLCPX; break;
      }
      type = dds_prime[DDS_CTYPE_INT].type;
      if(shape != DDS_SHAPE_S) {
         ddsYYError("unary '!' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_SIZEOF_EXPR_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST | DDS_SYM_DECL | DDS_SYM_INHIBIT |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_SIZE_T_DDS].type;
      if(type1->flag & (DDS_SYM_FUNC | DDS_SYM_FIELD | DDS_SYM_INCOMPLETE)) {
         ddsYYError("size not defined, operator 'sizeof' expression\n");
         flag |= DDS_SYM_ERROR;
         value_ptr = dds_error_value->ptr;
      } else {
         value_ptr = &type1->stroke;
      }
      break;
   case DDS_SIZEOF_TYPE_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST | DDS_SYM_DECL | DDS_SYM_INHIBIT |
         (value1->flag & DDS_SYM_ERROR));
#if defined(_WIN32) || defined(MINGW)
      type = dds_prime[HOST_LLONG_DDS].type;
      type1 = (DDS_TYPE*)value1;  /* deviant */
      assert(HOST_LLONG_P == sizeof(type1->stroke));
#else
      type = dds_prime[HOST_LONG_DDS].type;
      type1 = (DDS_TYPE*)value1;  /* deviant */
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
   case DDS_AXIS_SIZE_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST | DDS_SYM_DECL | DDS_SYM_INHIBIT |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_SIZE_T_DDS].type;
      if(genus1 == DDS_TYPE_CHARS) {
         for(axis = 1; axis <= dds_compile->cube.rank; axis++)
            if(! strcmp(dds_compile->cube.axis[axis].name, value1->ptr)) break;
         if (axis > dds_compile->cube.rank) {
            ddsYYError("axis_size argument is 'undefined': %s\n", value1->ptr);
            flag |= DDS_SYM_ERROR;
            axis = 1;
         }
      } else {
         axis = ddsSymConstInt(value1, 1, DDS_SYM_CONST_DECL);
      }
      if(axis < 1 || axis > dds_compile->cube.rank) {
         ddsYYError("axis_size argument is 'out-of-range': %i\n", axis);
         flag |= DDS_SYM_ERROR;
         value_ptr = dds_error_value->ptr;
      } else {
         value_ptr = &dds_compile->cube.axis[axis].size;
      }
      break;
   case DDS_ORDINAL_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_INHIBIT |
         DDS_SYM_CONST | DDS_SYM_VOLATILE |
         (value1->flag & DDS_SYM_ERROR));
      if(! (dds_state & DDS_YACC_IO)) {
         ddsYYError("ordinal function not defined in this context\n");
         flag |= DDS_SYM_ERROR;
      }
#if defined(_WIN32) || defined(MINGW)
      type = dds_prime[HOST_LLONG_DDS].type;
      assert(HOST_LLONG_P == sizeof(dds_compile->cube.axis[0].ordinal));
#else
      type = dds_prime[HOST_LONG_DDS].type;
      assert(HOST_LONG_P == sizeof(dds_compile->cube.axis[0].ordinal));
#endif
      if(genus1 == DDS_TYPE_CHARS) {
         for(axis = 1; axis <= dds_compile->cube.rank; axis++)
            if(! strcmp(dds_compile->cube.axis[axis].name, value1->ptr)) break;
         if (axis > dds_compile->cube.rank) {
            ddsYYError("ordinal argument is 'undefined': %s\n", value1->ptr);
            flag |= DDS_SYM_ERROR;
            axis = 1;
         }
      } else {
         axis = ddsSymConstInt(value1, 1, DDS_SYM_CONST_DECL);
      }
      if(axis < 1 || axis > dds_compile->cube.rank) {
         ddsYYError("ordinal argument is 'out-of-range': %i\n", axis);
         flag |= DDS_SYM_ERROR;
         value_ptr = dds_error_value->ptr;
      } else {
         value_ptr = &dds_compile->cube.axis[axis].ordinal;
      }
      break;
   case DDS_SQRT_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'sqrt' (square root)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'sqrt' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_SIN_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'sin' (sine)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'sin' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_COS_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'cos' (cosine)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'cos' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_TAN_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'tan' (tangent)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'tan' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_ASIN_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'asin' (arcsine)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'asin' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_ACOS_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'acos' (arccosine)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'acos' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_ATAN_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'atan' (argtangent)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'atan' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_EXP_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'exp' (exponential)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'exp' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_LOG_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'log' (natural log)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'log' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_LOG10_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'log10' (base-10 log)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'log10' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_ABS_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'abs' (absolute value)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'abs' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_SIGN_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'sign' (square root)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'sign' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_CEIL_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'ceil' (upper int)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'ceil' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_FLOOR_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[HOST_DOUBLE_DDS].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'floor' (lower int)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'floor' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_DATEINT_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[DDS_STYPE_INTEGER4].type;

      if(! ((1 << genus1) & DDS_STRUCT_OR_UNION_GENUS)) {
         ddsYYError("invalid operand type, function 'dateint' (date integer)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(value1->type->stroke != 10) {
         ddsYYError("invalid operand stroke=%d, function 'dateint'\n", 
            value1->type->stroke);
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'dateint' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   case DDS_DATESTRUCT_OP:
      flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
         (value1->flag & DDS_SYM_ERROR));
      type = dds_prime[DDS_CTYPE_ULLONG].type;
      value1 = ddsSymTypeCast(DDS_SYM_COERCE, type, value1);

      type = ddsSymTypeCopy(dds_prime[DDS_STYPE_STRUCT].type);
      type->flag |= DDS_SYM_STRUCT;
      type->prec = 10;
      type->align = 2;
      type->count = 1;
      type->stroke = 10;

      if(! ((1 << genus1) & DDS_ARITHMETIC_GENUS)) {
         ddsYYError("invalid operand type, function 'datestruct' (date structure)\n");
         flag |= DDS_SYM_ERROR;
      }
      if(shape != DDS_SHAPE_S) {
         ddsYYError("function 'datestruct' %s\n", dds_shape_name[shape]);
         flag |= DDS_SYM_ERROR;
      }
      break;
   default:
      ddsPanic("bad opcode\n");
   }

   if(! value_ptr) {
      value_ptr = ddsTablePush(dds_symbol, type->stroke, type->align);
   }

   expr = PUSH_EXPR1(dds_symbol, name, flag, dds_block, type);

   expr->ptr = value_ptr;
   expr->opcode = opcode;
   EXPR_ARG(&expr->arg1, value1);

   ddsSymExprDone((DDS_EXPR*)expr);

   return (DDS_VALUE*)expr;
}
