static char rcsid[]="$Id: symtypeshape2.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, test and reduce shape of two DDS_TYPE in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_SHAPE ddsSymTypeShape2(
   DDS_TYPE **type1a,
   DDS_TYPE **type2a)
{
   DDS_TYPE *type1b;
   DDS_TYPE *type2b;
   DDS_SHAPE shape;

   type1b = *type1a;
   type2b = *type2a;

   if(type1b->flag & DDS_SYM_ARRAY) {
      if(type2b->flag & DDS_SYM_ARRAY) {
         while((type1b->flag & DDS_SYM_ARRAY) &&
               (type2b->flag & DDS_SYM_ARRAY) &&
               type1b->count == type2b->count) {
               /* array dimension is conformable */
            type1b = type1b->type;  /* reduce dimension */
            type2b = type2b->type;  /* reduce dimension */
         }
         if((type1b->flag & DDS_SYM_ARRAY) ||
            (type2b->flag & DDS_SYM_ARRAY)) {
            /* non-conformable dimensions */
            shape = DDS_SHAPE_NON;
         } else {
            shape = DDS_SHAPE_VV;
         }
      } else {
         shape = DDS_SHAPE_VS;
         while(type1b->flag & DDS_SYM_ARRAY)
            type1b = type1b->type;
      }
   } else {
      if(type2b->flag & DDS_SYM_ARRAY) {
         shape = DDS_SHAPE_SV;
         while(type2b->flag & DDS_SYM_ARRAY)
            type2b = type2b->type;  /* reduce dimension */
      } else {
         shape = DDS_SHAPE_SS;
      }
   }

   *type1a = type1b;
   *type2a = type2b;

   return shape;
}
