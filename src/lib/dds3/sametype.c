deprecate, use ddsSymSameType

static char rcsid[]="$Id: sametype.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, test DDS_TYPE pair for "type" equivalence

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_BOOLEAN ddsSameType(
   DDS_TABLE *table1,
   DDS_TYPE *type1,
   DDS_TABLE *table2,
   DDS_TYPE *type2)
{
   DDS_SYM_FLAG flag1, flag2;
   DDS_SYM *sym1, *sym2;
   DDS_PTYPE stype1,stype2;
   DDS_EXPR *expr;
   DDS_OBJECT *object1, *object2;
   DDS_MEMBER *member1, *member2;
   DDS_BLOCK *block1, *block2;
   void *value1, *value2;

   flag1 = (DDS_SYM_FLAG)(type1->flag & (DDS_SYM_SUE));
   flag2 = (DDS_SYM_FLAG)(type2->flag & (DDS_SYM_SUE));
   if (!flag1 || !flag2) {
      if (flag1 || flag2) return DDS_FALSE;
      if (type1->align != type2->align) return DDS_FALSE;
      if (type1->count != type2->count) return DDS_FALSE;
      stype1 = dds_prime[type1->ptype].stype;
      stype2 = dds_prime[type2->ptype].stype;
      if (stype1 != stype2) return DDS_FALSE;
      return DDS_TRUE;
   }

   block1 = type1->block;
   block2 = type2->block;

   flag1 = (DDS_SYM_FLAG)(type1->flag & DDS_SYM_ENUM);
   flag2 = (DDS_SYM_FLAG)(type2->flag & DDS_SYM_ENUM);
   if (flag1 != flag2) return DDS_FALSE;

   if (flag1) {
      sym1 = (DDS_SYM*)block1->newer;
      sym2 = (DDS_SYM*)block2->newer;
      while(sym1 && sym2)
      {
         object1 = (DDS_OBJECT*)sym1;
         while (!object1->name && object1->newer) 
            object1 = (DDS_OBJECT*)object1->newer;
         object2 = (DDS_OBJECT*)sym2;
         while (!object2->name && object2->newer) 
            object2 = (DDS_OBJECT*)object2->newer;

         if (!object1->name) sym1 = 0;
         if (!object2->name) sym2 = 0;
         if (object1->type != block1->type) sym1 = 0;
         if (object2->type != block2->type) sym2 = 0;
         if (!sym1 || !sym2) break;

         expr   = object1->expr;
         type1  = expr->type;
         value1 = expr->value;
         expr   = object2->expr;
         type2  = expr->type;
         value2 = expr->value;

         if (type1->align != type2->align) return DDS_FALSE;
         if (type1->count != type2->count) return DDS_FALSE;
         stype1 = dds_prime[type1->ptype].stype;
         stype2 = dds_prime[type2->ptype].stype;
         if (stype1 != stype2) return DDS_FALSE;

         if (strncmp(value1, value2, type1->count * type1->prec))
            return DDS_FALSE;

         sym1 = (DDS_SYM*)object1->newer;
         sym2 = (DDS_SYM*)object2->newer;
      }
      while (sym1) {
         object1 = (DDS_OBJECT*)sym1;
         if (object1->name) {
            if (object1->type == block1->type) return DDS_FALSE; 
            break; 
         }
         sym1 = (DDS_SYM*)object1->newer;
      }
      while (sym2) {
         object2 = (DDS_OBJECT*)sym2;
         if (object2->name) {
            if (object2->type == block2->type) return DDS_FALSE; 
            break; 
         }
         sym2 = (DDS_SYM*)object2->newer;
      }
   }
   else {
      sym1 = block1->first;
      sym2 = block2->first;
      while(sym1 || sym2)
      {
         member1 = (DDS_MEMBER*)sym1;
         while (!member1->name && member1->newer) 
            member1 = (DDS_MEMBER*)member1->newer;
         member2 = (DDS_MEMBER*)sym2;
         while (!member2->name && member2->newer) 
            member2 = (DDS_MEMBER*)member2->newer;

         if (!member1->name) sym1 = 0;
         if (!member2->name) sym2 = 0;
         if (!sym1 || !sym2) break;

         type1 = member1->type;
         type2 = member2->type;

         if (type1->align != type2->align) return DDS_FALSE;
         if (type1->count != type2->count) return DDS_FALSE;
         stype1 = dds_prime[type1->ptype].stype;
         stype2 = dds_prime[type2->ptype].stype;
         if (stype1 != stype2) return DDS_FALSE;

         sym1 = (DDS_SYM*)member1->newer;
         sym2 = (DDS_SYM*)member2->newer;
      }
      if (sym1) {
         member1 = (DDS_MEMBER*)sym1;
         if (member1->name) return DDS_FALSE; 
         sym1= (DDS_SYM*)member1->newer;
      }
      while (sym2) {
         member2 = (DDS_MEMBER*)sym2;
         if (member2->name) return DDS_FALSE; 
         sym2 = (DDS_SYM*)member2->newer;
      }
   }

   return DDS_TRUE;
}
