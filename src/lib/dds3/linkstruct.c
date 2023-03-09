static char rcsid[]="$Id: linkstruct.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, find object and resolve it's type and structure

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsLinkStruct(
   DDS_STRUCT_REF *ref,
   DDS_IDX block_lnk,
   const char *ident)
{
   DDS_IDX object_lnk, type_lnk, struct_lnk;
   DDS_BLOCK *struct_ptr;
   DDS_OBJECT *object_ptr;
   DDS_TYPE *type_ptr;
   size_t prec;
   int rc = DDS_OK;

   strcpy(dds_symbol.data.ptr + dds_symbol.data.top, ident);
   object_lnk = ddsFindObject(dds_symbol.data.top, block_lnk);
   if(object_lnk) {
      object_ptr = OBJECT_PTR(object_lnk);
      type_lnk = object_ptr->type;
      type_ptr = TYPE_PTR(type_lnk);
      struct_lnk = type_ptr->block;
      struct_ptr = BLOCK_PTR(struct_lnk);
      prec = type_ptr->prec;
   } else {
      rc = DDS_BAD;
      type_lnk = 0;
      struct_lnk = 0;
      struct_ptr = NULL;
      prec = 0;
   }

   ref->object_lnk = object_lnk;
   ref->object_ptr = object_ptr;
   ref->type_lnk = type_lnk;
   ref->type_ptr = type_ptr;
   ref->prec = prec;
   ref->struct_lnk = struct_lnk;
   ref->struct_ptr = struct_ptr;

   return rc;
}
