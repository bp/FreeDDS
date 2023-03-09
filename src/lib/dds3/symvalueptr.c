static char rcsid[]="$Id: symvalueptr.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create relocatable value pointer

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

void ddsSymValuePtr(
   void **ptrptr,
   DDS_VALUE *value)
{
   DDS_EXPR2 *expr;
   DDS_VALUE **global;
   DDS_RELOC *reloc;
   DDS_SYM_FLAG flag;
   DDS_STACK *stack;
   size_t offset, idx;

   flag = value->flag;

   if(flag & DDS_SYM_ERROR) {
      /* fault, if "**ptrptr" is ever used */
      *ptrptr = dds_error_value->ptr;
   } else if((flag & DDS_RELOCATE_OBJECT) == DDS_RELOCATE_OBJECT ||
             (flag & DDS_RELOCATE_EXPR)   == DDS_RELOCATE_EXPR) {
      /* "**ptrptr" refers to a relocatable value */

      /* propagate relocation through INHIBITed expressions. */
      offset = 0;
      for(expr = (DDS_EXPR2*)value;
         ((expr->flag & DDS_RELOCATE_INHIBIT) == DDS_RELOCATE_INHIBIT);
         expr = (DDS_EXPR2*)expr->arg1.track) {
         /* Accumulate simple offsets provided by expression chain. */
         /* These expressions are INHIBITed; they are not evaluated. */
         offset += expr->arg2.value.idx;
      }
      value = (DDS_VALUE*)expr;

      if(! value->reloc) {
         /**********************************************************/
         /* This is the first reference to this relocatable value. */
         /* Push a pointer to this value onto the relocate stack.  */
         /* bin->relocate is used by ddsBinClose to cleanup stacks.*/
         /**********************************************************/
         idx = ddsStackPush(&dds_compile->relocate,
            sizeof(DDS_VALUE*), HOST_PTR_A);
         global = (DDS_VALUE**)ddsStackPtr(&dds_compile->relocate, idx);
         *global = value;

         /*************************************************************/
         /* Initialize relocation stack for references to this value. */
         /* It is an array of DDS_RELOC descriptors.  The second and  */
         /* subsequent elements describe pointers to this value.      */
         /*************************************************************/
         stack = (DDS_STACK*)ddsZalloc(sizeof(DDS_STACK));
         value->reloc = stack;
         ddsStackInit(stack, DDS_RELOC_MIN * sizeof(DDS_RELOC));

         /*************************************************************/
         /* The first entry on the value's relocate stack is deviant. */
         /* Its "ptrptr" is the last value ptr that was relocated.    */
         /* RELOC_VALUE relocates dependent pointers iff this ptrptr  */
         /* and the value ptr differ, i.e. iff things actual changed. */
         /*************************************************************/
         idx = ddsStackPush(stack, sizeof(DDS_RELOC), 1);
         reloc = (DDS_RELOC*)ddsStackPtr(stack, idx);
         reloc->ptrptr = (void**)dds_error_value->ptr;  /* deviant */
         reloc->offset = 0;  /* not applicable */
      } else {
         stack = value->reloc;
      }

      /* push another DDS_RELOC descriptor on value's dependency stack */
      idx = ddsStackPush(stack, sizeof(DDS_RELOC), 1);
      reloc = (DDS_RELOC*)ddsStackPtr(stack, idx);
      reloc->ptrptr = ptrptr;
      reloc->offset = offset;

      /* fault, if pointer is used before its relocated */
      *ptrptr = dds_error_value->ptr;
   } else {
      /* "value" is NOT relocatable ("*ptrptr" is constant) */
      *ptrptr = value->ptr;
   }

   return;
}
