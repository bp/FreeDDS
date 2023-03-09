static char rcsid[]="$Id: mapcompilesu.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, open map processing of structures & unions (compilation)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsMapCompileSU(
   DDS_MAP *map,
   DDS_STRING *out_prefix,
   long out_bias,
   DDS_TYPE *out_type,
   DDS_BOOLEAN map_default)
{
   DDS_SYM *sym;
   DDS_MEMBER *member;
   DDS_TYPE *type;
   size_t pre_size, tmp_size;
   char stri[8];
   int i, tmp_count;
   int status = DDS_OK;

   pre_size = strlen(out_prefix->ptr);

   for(sym = (DDS_SYM*)out_type->block->first;
      sym;
      sym = (DDS_SYM*)sym->newer) {

      if(! (sym->flag & DDS_SYM_MEMBER)) continue;

      member = (DDS_MEMBER*)sym;
      if(member->flag & DDS_SYM_FIELD) {
         ddsPanic("ddsMapCompileSU: struct/union bit fields deferred\n");
      }

      type = member->type;

      if(type->ptype == DDS_CTYPE_VOID
         || ! type->stroke)  /* zero dimension, good/bad idea ? */
         continue;

      /* map member to output, from input */
      out_prefix->ptr[pre_size] = '\0';
      if(type->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
         /* this member is itself a struct or union, recursive */
         if(type->count > 1) {
            ddsPanic("ddsMapCompileSU: struct/union arrays deferred\n");
         }

         /* check for any individual members first */
         ddsStrCat2(out_prefix, member->name, ".");
         if(DDS_OK != ddsMapCompileSU(map, out_prefix,
            out_bias + member->offset, type, map_default)) {
            /* "DDS_BAD" here simply means something was initialized. */
            /* This propagates up, inhibiting duplicate union maps. */
            status = DDS_BAD;
            if((out_type->flag & DDS_SYM_UNION) &&
               out_type->stroke == member->type->stroke) {
               /* Disable subsequent default mapping for this union, */
               /* because the entire thing has been initialized. */
               map_default = DDS_FALSE;
            }
         } else {
            /* otherwise, check if the entire structure/union is mapped */
            out_prefix->ptr[pre_size] = '\0';
            ddsStrCat(out_prefix, member->name);
            if(DDS_OK != ddsMapCompileSU2(map, out_prefix->ptr,
               out_bias + member->offset, member, map_default)) {
               /* "DDS_BAD" here simply means something was initialized. */
               /* This propagates up, inhibiting duplicate union maps. */
               status = DDS_BAD;
               if((out_type->flag & DDS_SYM_UNION) &&
                  out_type->stroke == member->type->stroke) {
                  /* Disable subsequent default mapping for this union, */
                  /* because the entire thing has been initialized. */
                  map_default = DDS_FALSE;
               }
            }
         }
      } else {
         /* member is a simple type (not struct/union) */
         ddsStrCat(out_prefix, member->name);
         if(DDS_OK != ddsMapCompileSU2(map, out_prefix->ptr,
            out_bias + member->offset, member, map_default)) {
            /* "DDS_BAD" here simply means something was initialized. */
            /* This propagates up, inhibiting duplicate union maps. */
            status = DDS_BAD;
            if((out_type->flag & DDS_SYM_UNION) &&
               out_type->stroke == member->type->stroke) {
               /* Disable subsequent default mapping for this union, */
               /* because the entire thing has been initialized. */
               map_default = DDS_FALSE;
            }
         } else if (member->type->count > 1) {
            /* This is an array: check each element */
            tmp_size = strlen(out_prefix->ptr);
            tmp_count = member->type->count;
            for (i=0;i<tmp_count;i++) {
               sprintf(stri, "%d", i);
               out_prefix->ptr[tmp_size] = '\0';
               ddsStrCat3(out_prefix, "[", stri, "]");
               if(DDS_OK != ddsMapCompileSU2(map, out_prefix->ptr,
                  out_bias + member->offset, member, map_default)) {
                  status = DDS_BAD;
               }
            }
         }
      }

#ifdef BAD_IDEA
      if(status == DDS_BAD && (out_type->flag & DDS_SYM_UNION)) {
         /* Stop mapping when ANY member of a union is mapped (BAD). */
         break;
      }
#endif
   }

   return status;
}
