static char rcsid[]="$Id: symexprdone.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, finalize DDS_EXPR thread in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_EXPR * ddsSymExprDone(
   DDS_EXPR *expr)
{
   /*********************************************************************/
   /* Constant expressions are evaluated once, when an expression       */
   /* is created in the symbol table.                                  */
   /*********************************************************************/

   if((expr->flag & DDS_SYM_INHIBIT) || (expr->flag & DDS_SYM_ERROR)) {
      /* no-op, if evaluation is inhibited or expression is bad. */
      return expr;
   } else if((expr->flag & DDS_SYM_CONST_DECL) &&
      ! (expr->flag & DDS_SYM_RELOC)) {
      /* Constant expression stored in a permanent location. */
      /* Optimize, evaluate it once at compile time. */
      ddsEvaluate(expr);
      return expr;
   }

   /* Thread this expression and evaluate it later. */
   if(dds_needle) {
      /* append this expression onto existing thread. */
      dds_needle->next = (DDS_EXPR4*)expr;
   } else {
      /* initial expression on evaluation thread. */
      dds_thread = expr;
   }
   dds_needle = expr;

   return expr;
}
