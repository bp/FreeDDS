static char rcsid[]="$Id: linkobjectsu.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, open map processing phase 1 (compilation)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsLinkObjectSU(
   DDS_BIN *bin,
   DDS_OBJECT *object,
   DDS_SEAM *seam,
   const char *mode,
   DDS_STRING *out_prefix,
   long out_bias,
   DDS_TYPE *out_type)
{
   DDS_SYM *sym;
   DDS_MEMBER *member;
   DDS_TYPE *type;
   size_t pre_size;
   int status = DDS_OK;

   pre_size = strlen(out_prefix->ptr);

   for(sym = (DDS_SYM*)out_type->block->first;
      sym;
      sym = (DDS_SYM*)sym->newer) {

      if(! (sym->flag & DDS_SYM_MEMBER)) continue;

      member = (DDS_MEMBER*)sym;
      if(member->flag & DDS_SYM_FIELD) {
         ddsPanic("ddsLinkObjectSU: struct/union bit fields deferred\n");
      }

      type = member->type;

      if(type->ptype == DDS_CTYPE_VOID
         || ! type->stroke)  /* zero dimension, good/bad idea ? */
         continue;

      /* map member to output, from itself */
      out_prefix->ptr[pre_size] = '\0';
      if(type->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
         /* this member is itself a struct or union, recursive */
         if(type->count > 1) {
            ddsPanic("ddsLinkObjectSU: struct/union arrays deferred\n");
         }
         ddsStrCat2(out_prefix, member->name, ".");
         if(DDS_OK != ddsLinkObjectSU(bin, object, seam, mode,
            out_prefix, out_bias + member->offset, type)) {
            status = DDS_BAD;
         }
      } else {
         /* member is a simple type (not struct/union) */
         ddsStrCat(out_prefix, member->name);
         if(DDS_OK != ddsLinkObjectSU2(bin, object, seam, mode,
            out_prefix->ptr, out_bias)) {
            status = DDS_BAD;
         }
      }
   }

   return status;
}
