static char rcsid[]="$Id: objecttag.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, find object and create its API tag

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsObjectTag(
   DDS_OBJECT_REF *ref,
   DDS_BLOCK *block,
   const char *ident)
{
   DDS_OBJECT *object;
   SYM_TAG tag;
   int status = DDS_OK;

   object = LOOKUP_OBJECT(dds_symbol, ident, block);

   if(! object || (object->flag & DDS_SYM_ERROR)) {
      status = DDS_BAD;
      object = dds_error_object;
   }

/*** both are needed ?
***/
   SYM_TO_SYM_TAG(tag, &dds_compile->sym_tag, object)
   SYM_TO_SYM_TAG(tag, &dds_compile->sym_tag, object->type)

   ref->object = object;
   ref->type = object->type;

   return status;
}
