static char rcsid[]="$Id: symtypeshape.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, test and reduce shape of one DDS_TYPE in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_SHAPE ddsSymTypeShape(
   DDS_TYPE **type1a)
{
   DDS_TYPE *type1b;
   DDS_SHAPE shape;

   type1b = *type1a;

   if(type1b->flag & DDS_SYM_ARRAY) {
      shape = DDS_SHAPE_V;
      while(type1b->flag & DDS_SYM_ARRAY)
         type1b = type1b->type;  /* reduce dimension */
   } else {
      shape = DDS_SHAPE_S;
   }

   *type1a = type1b;

   return shape;
}
