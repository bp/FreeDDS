static char rcsid[]="$Id: symexpr3a.c 67 2009-03-06 23:03:49Z user $";
/*
RCS-t=DDS, create DDS_EXPR (three arguments '?:') in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

/* True, if assignment operator is eliminated. */
#define EXPR1_HAS_NO_SIDE_EFFECTS

DDS_VALUE * ddsSymExpr3A(
   const char *name,
   DDS_OPCODE opcode,
   DDS_VALUE *value1,
   DDS_VALUE *value2,
   DDS_VALUE *value3,
   DDS_EXPR *needle1,
   DDS_EXPR *needle2,
   DDS_EXPR *needle3)
{
   DDS_SYM_FLAG flag;
   DDS_PTYPE concur;
   DDS_VALUE *negate1, *const_value;
   DDS_EXPR4 *expr;
   DDS_EXPR *save_thread, *save_needle;
   DDS_EXPR *noodle1, *noodle2, *noodle3;
   DDS_CONDITION *condition;
   DDS_EARG arg4;
   DDS_TYPE *type, *type2, *type3;
   DDS_TYPE *type1a, *type2a, *type3a;
   DDS_TYPE_GENUS genus2a, genus3a;
   DDS_SHAPE shape1, shape23;

   /****************************************************/
   /* argument usage                                   */
   /*    arg1.value.ptr == condition                   */
   /*    arg2.value.ptr == true  branch value          */
   /*    arg3.value.ptr == false branch value          */
   /*    arg4.value.ptr == (DDS_CONDITION*)            */
   /****************************************************/

   assert(opcode == DDS_CONDITIONAL_OP);

   type1a = value1->type;
   type2a = type2 = value2->type;
   type3a = type3 = value3->type;

   condition = (DDS_CONDITION*)ddsTablePush(dds_symbol,
      sizeof(DDS_CONDITION), HOST_MALLOC_A);
   memset((void*)condition, '\0', sizeof(DDS_CONDITION));
   arg4.value.ptr = condition;
   arg4.track = NULL;

   /******************************************************/
   /* All this needle/thread mechanism is all messed up. */
   /* So skip it all for now - J. Ehlers Mar 2009        */
   /******************************************************/

#ifdef DDS_USE_CONDITIONAL_NEEDLE_THREAD

   /* unravel the evaluation thread into three branches (max) */
   noodle1 = noodle2 = noodle3 = NULL;
   if(needle1) {
      if(needle1 != needle2) {
         condition->expr1 = (DDS_EXPR*)needle1->next;
         needle1->next = NULL;
         noodle1 = needle2;
         if(needle2 != needle3 && needle2) {
            condition->expr2 = (DDS_EXPR*)needle2->next;
            needle2->next = NULL;
            noodle2 = needle3;
            if(needle3 != dds_needle) {
               condition->expr3 = (DDS_EXPR*)needle3->next;
               needle3->next = NULL;
               noodle3 = dds_needle;
            }
         }
         dds_needle = needle1;
      } else if(needle2 != needle3) {
         condition->expr2 = (DDS_EXPR*)needle2->next;
         needle2->next = NULL;
         noodle2 = needle3;
         if(needle3 != dds_needle) {
            condition->expr3 = (DDS_EXPR*)needle3->next;
            needle3->next = NULL;
            noodle3 = dds_needle;
         }
         dds_needle = needle2;
      } else if(needle3 != dds_needle) {
         condition->expr3 = (DDS_EXPR*)needle3->next;
         needle3->next = NULL;
         noodle3 = dds_needle;
         dds_needle = needle3;
      }
   }


   /* save thread and needle required for run time evaluation */
   save_thread = dds_thread;
   save_needle = dds_needle;

   /* compute boolean for value1 by negating it */
   dds_thread = condition->expr1;
   dds_needle = noodle1;
   negate1 = ddsSymExpr1A(NULL, DDS_NEGATION_OP_INT, value1);
   condition->expr1 = dds_thread;
   noodle1 = dds_needle;

   flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
      (value2->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION | DDS_SYM_ARRAY)) |
      ((negate1->flag | value2->flag | value3->flag) &
         (DDS_SYM_ERROR | DDS_SYM_DECL | DDS_SYM_INCOMPLETE)) |
      ((value2->flag | value3->flag) & DDS_SYM_RELOC) |
      ((value2->flag & value3->flag) & DDS_SYM_ENUM));

   shape1 = ddsSymTypeShape(&type1a);
   shape23 = ddsSymTypeShape2(&type2a, &type3a);

   concur = dds_concur[type2a->ptype][type3a->ptype];

   genus2a = dds_prime[type2->ptype].genus;
   genus3a = dds_prime[type3->ptype].genus;

   if(shape1 != DDS_SHAPE_S) {
      ddsYYError("conditional '?:' %s\n", dds_shape_name[shape1]);
      flag |= DDS_SYM_ERROR;
      type = dds_error_type;
   }

   if(shape23 != DDS_SHAPE_SS && shape23 != DDS_SHAPE_VV) {
      ddsYYError("incompatible operand shapes, '?:' conditional\n");
      flag |= DDS_SYM_ERROR;
      type = dds_error_type;
   } else if(shape23 != DDS_SHAPE_SS) {
      /* temporary constraint */
      ddsYYError("%s, '?:' conditional\n", dds_shape_name[shape23]);
      flag |= DDS_SYM_ERROR;
      type = dds_error_type;
   }

   if(flag & DDS_SYM_ERROR) {
      type = dds_error_type;
   } else if(((genus2a == DDS_TYPE_STRUCTS && genus3a == DDS_TYPE_STRUCTS) ||
              (genus2a == DDS_TYPE_UNIONS  && genus3a == DDS_TYPE_UNIONS)) &&
       ddsSymSameType(type2, type3)) {
      type = type2;
   } else if(concur != DDS_CTYPE_VOID) {
      type = dds_prime[concur].type;
      /* cast the true branch to the expression type */
      dds_thread = condition->expr2;
      dds_needle = noodle2;
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);
      condition->expr2 = dds_thread;
      noodle2 = dds_needle;

      /* cast the false branch to the expression type */
      dds_thread = condition->expr3;
      dds_needle = noodle3;
      value3 = ddsSymTypeCast(DDS_SYM_COERCE, type, value3);
      condition->expr3 = dds_thread;
      noodle3 = dds_needle;
   } else {
      ddsYYError("incompatible types, '?:' conditional\n");
      flag |= DDS_SYM_ERROR;
      type = dds_error_type;
   }

   dds_thread = save_thread;
   dds_needle = save_needle;
   const_value = NULL;

   if(! (flag & DDS_SYM_ERROR)) {
      if(negate1->flag & DDS_SYM_CONST_DECL) {
         /* optimize, decision is a compile time constant */
         /*         flag |= DDS_SYM_INHIBIT; */
         #ifdef EXPR1_HAS_NO_SIDE_EFFECTS
            /* don't bother to thread expr1 for evaluation */
         #else /* EXPR1_HAS_NO_SIDE_EFFECTS */
            if(condition->expr1) {
               if(dds_needle) {
                  /* append expression onto existing thread. */
                  dds_needle->next = (DDS_EXPR4*)condition->expr1;
               } else {
                  /* initial expression on evaluation thread. */
                  dds_thread = condition->expr1;
               }
               dds_needle = noodle1;
            }
         #endif /* EXPR1_HAS_NO_SIDE_EFFECTS */
         if(condition->expr1) ddsEvaluate(condition->expr1);
         if(*(int*)negate1->ptr) {
            /* false branch is *always* selected */
            if(value3->flag & DDS_SYM_CONST_DECL) {
               /* optimize, false branch is a compile time constant */
               flag |= DDS_SYM_INHIBIT; 
               if(condition->expr3) ddsEvaluate(condition->expr3);
               const_value = value3;
            } else if(condition->expr3) {
               /* thread false branch for run time evaluation */
               if(dds_needle) {
                  /* append expression onto existing thread. */
                  dds_needle->next = (DDS_EXPR4*)condition->expr3;
               } else {
                  /* initial expression on evaluation thread. */
                  dds_thread = condition->expr3;
               }
               dds_needle = noodle3;
            }
         } else {
            /* true branch is *always* selected */
            if(value2->flag & DDS_SYM_CONST_DECL) {
               /* optimize, true branch is a compile time constant */
               flag |= DDS_SYM_INHIBIT; 
               if(condition->expr2) ddsEvaluate(condition->expr2);
               const_value = value2;
            } else if(condition->expr2) {
               /* thread false branch for run time evaluation */
               if(dds_needle) {
                  /* append expression onto existing thread. */
                  dds_needle->next = (DDS_EXPR4*)condition->expr2;
               } else {
                  /* initial expression on evaluation thread. */
                  dds_thread = condition->expr2;
               }
               dds_needle = noodle2;
            }
         }
      } else {
         /* decision requires run time evaluation */
         /* (Force it to be relocatable to get it to work right!!) */
         flag |= DDS_SYM_RELOC;
         if(condition->expr1) {
            if(dds_needle) {
               /* append expression onto existing thread. */
               dds_needle->next = (DDS_EXPR4*)condition->expr1;
            } else {
               /* initial expression on evaluation thread. */
               dds_thread = condition->expr1;
            }
            dds_needle = noodle1;
         }
         if(condition->expr2) {
            if(dds_needle) {
               /* append expression onto existing thread. */
               dds_needle->next = (DDS_EXPR4*)condition->expr2;
            } else {
               /* initial expression on evaluation thread. */
               dds_thread = condition->expr2;
            }
            dds_needle = noodle2;
         }
         if(condition->expr3) {
            if(dds_needle) {
               /* append expression onto existing thread. */
               dds_needle->next = (DDS_EXPR4*)condition->expr3;
            } else {
               /* initial expression on evaluation thread. */
               dds_thread = condition->expr3;
            }
            dds_needle = noodle3;
         }
      }
   }

#else
   negate1 = ddsSymExpr1A(NULL, DDS_NEGATION_OP_INT, value1);

   flag = (DDS_SYM_FLAG)(DDS_SYM_CONST |
      (value2->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION | DDS_SYM_ARRAY)) |
      ((negate1->flag | value2->flag | value3->flag) &
         (DDS_SYM_ERROR | DDS_SYM_DECL | DDS_SYM_INCOMPLETE)) |
      ((value2->flag | value3->flag) & DDS_SYM_RELOC) |
      ((value2->flag & value3->flag) & DDS_SYM_ENUM));

   shape1 = ddsSymTypeShape(&type1a);
   shape23 = ddsSymTypeShape2(&type2a, &type3a);

   concur = dds_concur[type2a->ptype][type3a->ptype];

   genus2a = dds_prime[type2->ptype].genus;
   genus3a = dds_prime[type3->ptype].genus;

   if(shape1 != DDS_SHAPE_S) {
      ddsYYError("conditional '?:' %s\n", dds_shape_name[shape1]);
      flag |= DDS_SYM_ERROR;
      type = dds_error_type;
   }

   if(shape23 != DDS_SHAPE_SS && shape23 != DDS_SHAPE_VV) {
      ddsYYError("incompatible operand shapes, '?:' conditional\n");
      flag |= DDS_SYM_ERROR;
      type = dds_error_type;
   } else if(shape23 != DDS_SHAPE_SS) {
      /* temporary constraint */
      ddsYYError("%s, '?:' conditional\n", dds_shape_name[shape23]);
      flag |= DDS_SYM_ERROR;
      type = dds_error_type;
   }

   if(flag & DDS_SYM_ERROR) {
      type = dds_error_type;
   } else if(((genus2a == DDS_TYPE_STRUCTS && genus3a == DDS_TYPE_STRUCTS) ||
              (genus2a == DDS_TYPE_UNIONS  && genus3a == DDS_TYPE_UNIONS)) &&
       ddsSymSameType(type2, type3)) {
      type = type2;
   } else if(concur != DDS_CTYPE_VOID) {
      type = dds_prime[concur].type;
      /* cast the true branch to the expression type */
      value2 = ddsSymTypeCast(DDS_SYM_COERCE, type, value2);

      /* cast the false branch to the expression type */
      value3 = ddsSymTypeCast(DDS_SYM_COERCE, type, value3);
   } else {
      ddsYYError("incompatible types, '?:' conditional\n");
      flag |= DDS_SYM_ERROR;
      type = dds_error_type;
   }

   const_value = NULL;

   /* decision requires run time evaluation */
   /* (Force it to be relocatable to get it to work right!!) */
   flag |= DDS_SYM_RELOC;

#endif

   expr = PUSH_EXPR4(dds_symbol, name, flag, dds_block, type);

   if(const_value) {
      ddsSymValuePtr(&expr->ptr, const_value);
   } else {
       expr->ptr = NULL;
       /*  ddsSymValuePtr(&expr->ptr, value1); */
   }

   expr->opcode = opcode;
   EXPR_ARG(&expr->arg1, value1);
   EXPR_ARG(&expr->arg2, value2);
   EXPR_ARG(&expr->arg3, value3);
   expr->arg4 = arg4;

   ddsSymExprDone((DDS_EXPR*)expr);

   return (DDS_VALUE*)expr;
}
