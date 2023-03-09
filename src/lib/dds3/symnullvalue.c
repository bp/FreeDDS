static char rcsid[]="$Id: symnullvalue.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, test DDS_OBJECT for a NULL value (arithmetic constant assumed)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_BOOLEAN ddsSymNullValue(
   DDS_VALUE *value)
{
   DDS_TYPE *type;
   const char *buf;
   size_t stroke;

   type = value->type;
   stroke = type->stroke;
   buf = value->ptr;

   while(stroke--) if(*buf++) return DDS_FALSE;

   return DDS_TRUE;
}
