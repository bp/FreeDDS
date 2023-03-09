static char rcsid[]="$Id: mapassign.c 248 2012-09-05 15:17:42Z user $";
/*
RCS-t=DDS API, open map assignments

*/

/**********************************************/
/***   Copyright (C) 2006                   ***/
/***   BP America, Inc, Houston, TX         ***/
/***   Randy Selzler, xx July 1996          ***/
/***   Jerry Ehlers, August 2006            ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

void ddsMapAssign(
   const char *expr_name,
   DDS_BIN *out_bin,
   DDS_OBJECT *out_object,
   const char *out_name,
   DDS_VALUE *rvalue,
   DDS_OBJECT *in_object,
   DDS_SEAM *seam)
{
   DDS_VALUE *lvalue;
   const char *name;
   int rank, rank_max;

   /* Skip mapping Samples if cddx_out or cddx_outhdr is mapping headers only */
   if (dds_map_samples==0 && strstr(out_name,"Samples")) return;

   /* This output member IS mapped. */
   rank_max = out_bin->cube.rank;
   for(rank = out_bin->cube.io_rank; rank <= rank_max; rank++) {
      if(! strcmp(out_name, out_bin->cube.axis[rank].sort_name)) {
         /* This output member IS a sort field that is being mapped. */
         out_bin->cube.axis[rank].flag |= DDS_AXIS_SORT_SET;
         break;
      }
   }

   /* Select the symbol table for the output binary. */
   dds_compile = out_bin;
   dds_symbol = &out_bin->symbol;
   dds_block = out_object->block;

   dds_lexer = dds_lexer->inner; /* crude, but effective */

   /* Create lvalue expression (output member reference). */
   lvalue = ddsSymExpr2C(NULL, DDS_STRUCT_MEMBER_OP,
      (DDS_VALUE*)out_object, out_name);

   /* Create assignment expression (lvalue = rvalue) */
   name = ddsTablePushStr(dds_symbol, expr_name);
   ddsSymExpr2A(name, DDS_ASSIGN_OP, lvalue, rvalue);

   dds_lexer = dds_lexer->outer; /* crude, but effective */

   /* Append another expression on seam's thread */
   if(seam->thread && seam->needle) {
      /* Append new expressions onto existing thread. */
      seam->needle->next = (DDS_EXPR4*)dds_thread;
   } else {
      /* Initial thread for seam expressions. */
      seam->thread = dds_thread;
   }
   seam->needle = dds_needle;
   dds_thread = 0;
   dds_needle = 0;

   return;
}
