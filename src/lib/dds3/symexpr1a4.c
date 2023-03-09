static char rcsid[]="$Id: symexpr1a4.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_EXPR (one argument, unary ops) in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

DDS_VALUE * ddsSymExpr1A4(
   const char *name1,
   DDS_VALUE *value2)
{
   DDS_VALUE *value;
#if defined(_WIN32) || defined(MINGW)
   size_t bias;
#else
   long bias;
#endif

   if(dds_map_value) {
      if(ddsSymLookupMem(dds_symbol, name1,
         dds_map_value->type->block, &bias)) {
         /* TRUE, name1 is a member of struct/union */
         value = ddsSymExpr2C(NULL, DDS_STRUCT_MEMBER_OP,
            dds_map_value, name1);
      } else {
         /* FALSE, name1 is not a member of struct/union */
         value = value2;
      }
   } else {
      ddsYYError("'field' not defined in this context\n");
      value = dds_error_value;
   }

   return value;
}
