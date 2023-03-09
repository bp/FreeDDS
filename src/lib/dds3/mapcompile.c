static char rcsid[]="$Id: mapcompile.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, open map processing phase 1 (compilation)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsMapCompile(
   DDS_MAP *map)
{
   static DDS_STRING out_prefix = NULL_STRING;
   DDS_TYPE *out_type, *in_type;
   DDS_BIN *out_bin, *in_bin;
   int status = DDS_OK;

   assert(! dds_thread && ! dds_needle);

   if(! out_prefix.ptr) {
      ddsStrSetMin(&out_prefix, "", 64);
   }

   out_bin = map->out_bin;
   in_bin = map->in_bin;

   out_type = map->out_object->type;
   in_type = map->in_object->type;

   if(in_type->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
      /* "map_block" is needed by scanner to find input members */
      dds_map_value = (DDS_VALUE*)map->in_object;
   } else {
      dds_map_value = NULL;
   }

   /* Map creation is dependent upon the output type. */
   /*    If its a struct, each member is mapped individually. */
   /*    If its a union, one and only one member may be mapped. */
   /*    If its a non-structured type, ... ? */

   if(out_type->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
      /* map struct/union members */
      *out_prefix.ptr = '\0';
      if(DDS_OK != ddsMapCompileSU(map, &out_prefix, 0, out_type, DDS_TRUE)) {
         status = DDS_BAD;
      }
      if(! map->seam.needle) {
         ddsVerbose("cdds_openm: nothing mapped from input to output\n\tout_bin= %s, out_type= %s\n\tin_bin= %s, in_type= %s\n",
            out_bin->name, ddsHintType(out_bin, out_type),
             in_bin->name, ddsHintType( in_bin,  in_type));
      }
   } else if(out_type->ptype != DDS_CTYPE_VOID) {
      /* ddsMapVect(...) */
      ddsPanic("cdds_openm: strange out_genus deferred\n");
   } else {
      ddsError("cdds_openm: invalid type of output\n");
      status = DDS_BAD;
   }

   dds_map_value = NULL;

   return status;
}
