static char rcsid[]="$Id: dumpexpr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_EXPR

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpExpr(
   int indent,
   DDS_EXPR4 *expr)
{
   DDS_EXPRA *expra;
   DDS_EXPRC *exprc;

   if(! dds_initialized) return; /* no-op */

   if(! expr) {
      fprintf(dds_print_fp, "ddsDumpExpr: NULL EXPR ptr\n");
      return;
   }

   fprintf(dds_print_fp, "EXPR ");
   ddsDumpSym(indent, (DDS_SYM*)expr);

   fprintf(dds_print_fp, "%*sopcode= %12s; ptr= %p; reloc= %p; next= %p;\n",
      indent, "", dds_opnote[expr->opcode].name,
                  (void*)expr->ptr,
                  (void*)expr->reloc,
                  (void*)expr->next);

   if(dds_opnote[expr->opcode].args) {
      fprintf(dds_print_fp, "%*s", indent, "");
      if(dds_opnote[expr->opcode].args >= 1) {
         if(dds_opnote[expr->opcode].flag & DDS_OPNOTE_ARG1) {
            fprintf(dds_print_fp, "arg1= %p(%p);",
               expr->arg1.value.ptr, expr->arg1.track);
         } else {
            fprintf(dds_print_fp, "arg1= %5li;",
               (long)expr->arg1.value.idx);
         }
         if(dds_opnote[expr->opcode].args >= 2) {
            if(dds_opnote[expr->opcode].flag & DDS_OPNOTE_ARG2) {
               fprintf(dds_print_fp, " arg2= %p(%p);",
                  expr->arg2.value.ptr, expr->arg2.track);
            } else {
               fprintf(dds_print_fp, " arg2= %5li;",
                  (long)expr->arg2.value.idx);
            }
            if(dds_opnote[expr->opcode].args >= 3) {
               if(dds_opnote[expr->opcode].flag & DDS_OPNOTE_ARG3) {
                  fprintf(dds_print_fp, " arg3= %p(%p);",
                     expr->arg3.value.ptr, expr->arg3.track);
               } else {
                  fprintf(dds_print_fp, " arg3= %5li;",
                     (long)expr->arg3.value.idx);
               }
               if(dds_opnote[expr->opcode].args >= 4) {
                  if(dds_opnote[expr->opcode].flag & DDS_OPNOTE_ARG4) {
                     fprintf(dds_print_fp, " arg4= %p(%p);",
                        expr->arg4.value.ptr, expr->arg4.track);
                  } else {
                     fprintf(dds_print_fp, " arg4= %5li;",
                        (long)expr->arg4.value.idx);
                  }
               } else if(expr->opcode == 1) {  /* DDS_COPY_OP */
                  exprc = (DDS_EXPRC*)expr;
                  fprintf(dds_print_fp, " lhs_bias= %li; rhs_bias= %li;",
                     (long)exprc->lhs_bias,
                     (long)exprc->rhs_bias);
               }
            } else if(expr->opcode == 0) {  /* DDS_ASSIGN_OP */
               expra = (DDS_EXPRA*)expr;
               fprintf(dds_print_fp, " stype1= %s; 2= %s; count= %li;",
                  dds_prime[expra->stype1].macro,
                  dds_prime[expra->stype2].macro,
                  (long)expra->count);
            }
         }
      }
      fprintf(dds_print_fp, "\n");
   }

   ddsDumpReloc(indent, (DDS_VALUE*)expr);

   return;
}
