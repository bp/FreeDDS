static char rcsid[]="$Id: symmemberbit.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create struct/union DDS_MEMBER bit field in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <limits.h>
#include "dds_sys.h"

DDS_MEMBER * ddsSymMemberBit(
   DDS_MEMBER *member,
   int adjust)
{
   DDS_TYPE *type;

   type = dds_declare->type;
   if(type->flag & DDS_SYM_FIELD) {
      ddsYYError("multiple bit field widths\n");
      member->flag |= DDS_SYM_ERROR;
   } else {
      if(type->ptype != DDS_CTYPE_INT &&
         type->ptype != DDS_CTYPE_UINT) {
         ddsYYError("invalid type for bit field\n");
         member->flag |= DDS_SYM_ERROR;
      } else if((adjust > CHAR_BIT * HOST_INT_P) ||
         (member->name && ! adjust)) {
         ddsYYError("invalid size for bit field\n");
         member->flag |= DDS_SYM_ERROR;
      } else {
         ddsYYError("bit field support is deferred\n");
         member->flag |= DDS_SYM_ERROR;
         member->flag |= DDS_SYM_FIELD;
         member->adjust = adjust;
      }
   }

   return member;
}
