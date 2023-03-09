static char rcsid[]="$Id: linkobject.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, find object and compile map for read/write/seek processing

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsLinkObject(
   DDS_OBJECT_REF *ref)
{
   static DDS_STRING out_prefix = NULL_STRING;
   static DDS_STRING map_name = NULL_STRING;
   DDS_OBJECT *object;
   DDS_TYPE *out_type;
   const char *name;
   int status = DDS_OK;

   if(! out_prefix.ptr) {
      ddsStrSetMin(&out_prefix, "", 64);
   }

   object = ref->object;

   if(! (object->flag & DDS_SYM_ERROR)) {
      /* create maps for read/write/seek processing */
      out_type = object->type;
      dds_state |= (DDS_SYM_FLAG)(DDS_YACC_MAP | DDS_YACC_IO);

      /* create unique block for symbols. */
      ddsStrCpyCat2(&map_name, "read/write/seek:", object->name);
      name = ddsTablePushStr(dds_symbol, map_name.ptr);
      ddsSymBlockBegin(name, (DDS_SYM_FLAG)0, NULL);

      /* create objects for buffers */
      if(dds_compile->media->state & DDS_MEDIA_OPENR) {
         ref->r_object = PUSH_OBJECT(dds_symbol, "<read object>",
            (DDS_SYM_FLAG)(DDS_SYM_REGISTER | DDS_SYM_RELOC | DDS_SYM_LVALUE),
            dds_block, object->type);
      }

      if(dds_compile->media->state & DDS_MEDIA_OPENW) {
         ref->w_object = PUSH_OBJECT(dds_symbol, "<write object>",
            (DDS_SYM_FLAG)(DDS_SYM_REGISTER | DDS_SYM_RELOC | DDS_SYM_LVALUE),
            dds_block, object->type);
      }

      dds_map_value = NULL;

      if(out_type->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
         /* map struct/union members */
         if(dds_compile->media->state & DDS_MEDIA_OPENR) {
            *out_prefix.ptr = '\0';
            dds_map_value = (DDS_VALUE*)ref->r_object;
            if(DDS_OK != ddsLinkObjectSU(dds_compile, ref->r_object,
               &ref->r_seam, "read", &out_prefix, 0, out_type)) {
               status = DDS_BAD;
            }
         }

         if(dds_compile->media->state & DDS_MEDIA_OPENW) {
            *out_prefix.ptr = '\0';
            dds_map_value = (DDS_VALUE*)ref->w_object;
            if(DDS_OK != ddsLinkObjectSU(dds_compile, ref->w_object,
               &ref->w_seam, "write", &out_prefix, 0, out_type)) {
               status = DDS_BAD;
            }
         }
      } else if(out_type->ptype != DDS_CTYPE_VOID) {
         /* ddsMapVect(...) */
         ddsPanic("cdds_openm: strange out_genus deferred\n");
      } else {
         ddsError("cdds_openm: invalid type of output\n");
         status = DDS_BAD;
      }

      DECLARE_INNER; /* kludge for ddsSymBlockEnd */

      /* exit the block for symbols. */
      ddsSymBlockEnd();

      dds_map_value = NULL;
      dds_state &= (DDS_SYM_FLAG) ~ (DDS_YACC_MAP | DDS_YACC_IO);
   }

   return status;
}
