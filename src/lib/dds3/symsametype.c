static char rcsid[]="$Id: symsametype.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, test DDS_TYPE pair for "type" equivalence

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_BOOLEAN ddsSymSameType(
   DDS_TYPE *type1,
   DDS_TYPE *type2)
{
   DDS_SYM *sym1, *sym2;
   DDS_MEMBER *member1, *member2;
   DDS_OBJECT *object1, *object2;
   DDS_SYM_FLAG mask;

   if(type1 == type2) return DDS_TRUE;

   mask = (DDS_SYM_FLAG)(DDS_SYM_ERROR | DDS_SYM_INCOMPLETE |
      DDS_SYM_ASUE | DDS_SYM_CONST | DDS_SYM_VOLATILE | DDS_SYM_ALIGN);

   if((type1->flag & mask) != (type2->flag & mask) ||
      type1->ptype  != type2->ptype  ||
      type1->align  != type2->align  ||
      type1->prec   != type2->prec   ||
      type1->count  != type2->count  ||
      type1->stroke != type2->stroke) return DDS_FALSE;

   if(! (type1->flag & DDS_SYM_SUE)) return DDS_TRUE;

   /******************************************************/
   /* Identify structure as the same type if we get here */
   /* (to allow datestruct() to work on a structure)     */
   /******************************************************/
   if (type1->flag & DDS_SYM_STRUCT) return DDS_TRUE;

   /* compare struct, union, or enum */
   if(type1->block->name && type2->block->name) {
      if(strcmp(type1->block->name, type2->block->name)) return DDS_FALSE;
   } else if(type1->block->name || type2->block->name) {
      return DDS_FALSE;
   }

   if(type1->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
      /* compare struct or union members */
      sym1 = type1->block->first;
      sym2 = type2->block->first;
   
      /* examine all symbols in both blocks. */
      while(sym1 || sym2) {
         /* find next member in type1 */
         while(sym1 && ! (sym1->flag & DDS_SYM_MEMBER))
            sym1 = (DDS_SYM*)sym1->newer;

         /* find next member in type2 */
         while(sym2 && ! (sym2->flag & DDS_SYM_MEMBER))
            sym2 = (DDS_SYM*)sym2->newer;

         if(sym1 && sym2) {
            /* compare another pair */
            member1 = (DDS_MEMBER*)sym1;
            member2 = (DDS_MEMBER*)sym2;

            if(member1->offset != member2->offset ||
               member1->adjust != member2->adjust) return DDS_FALSE;

            if(member1->name && member2->name) {
               if(strcmp(member1->name, member2->name)) return DDS_FALSE;
            } else if(member1->name || member2->name) {
               return DDS_FALSE;
            }

            if(! ddsSymSameType(member1->type, member2->type))
               return DDS_FALSE;

         } else if(sym1 || sym2) {
            /* members not paired */
            return DDS_FALSE;
         }
         sym1 = (DDS_SYM*)sym1->newer;
         sym2 = (DDS_SYM*)sym2->newer;
      }
   } else {
      /* compare enum names and values. */
      sym1 = type1->block->block->first;
      sym2 = type2->block->block->first;

      /* examine all symbols in both blocks. */
      while(sym1 || sym2) {
         /* find next enumeration in block1 */
         while(sym1 && (! (sym1->flag & DDS_SYM_OBJECT) ||
            ! (sym1->flag & DDS_SYM_ENUM) || sym1->type != type1))
            sym1 = (DDS_SYM*)sym1->newer;

         /* find next enumeration in block2 */
         while(sym2 && (! (sym2->flag & DDS_SYM_OBJECT) ||
            ! (sym2->flag & DDS_SYM_ENUM) || sym2->type != type2))
            sym2 = (DDS_SYM*)sym2->newer;

         if(sym1 && sym2) {
            /* compare another pair */
            object1 = (DDS_OBJECT*)sym1;
            object2 = (DDS_OBJECT*)sym2;

            if(strcmp(object1->name, object2->name))
               return DDS_FALSE;

            if(memcmp(object1->ptr, object2->ptr, type1->prec))
               return DDS_FALSE;

         } else if(sym1 || sym2) {
            /* enumerations not paired */
            return DDS_FALSE;
         }
         sym1 = (DDS_SYM*)sym1->newer;
         sym2 = (DDS_SYM*)sym2->newer;
      }
   }

   return DDS_TRUE;
}
