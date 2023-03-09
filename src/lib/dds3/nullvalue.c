deprecate, use ddsSymNullValue instead

static char rcsid[]="$Id: nullvalue.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, test DDS_OBJECT for a NULL value (arithmetic constant assumed)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_BOOLEAN ddsNullValue(
   DDS_TABLE *table,
   DDS_EXPR *expr)
{
   const char *buf;
   DDS_TYPE *type;
   size_t stroke;

   while(expr->flag & DDS_SYM_IGNORE)
      expr = (DDS_EXPR*)expr->value;

   type = expr->type;
   stroke = type->stroke;
   buf = expr->value;

   while(stroke--) if(*buf++) return DDS_FALSE;

   return DDS_TRUE;
}
