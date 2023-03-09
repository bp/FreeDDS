static char rcsid[]="$Id: c_map.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, map binary data to output buffer from input buffer

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_map(
   BIN_TAG out_bin_tag,
   SYM_TAG out_sym_tag,
   void *out_buf,
   BIN_TAG in_bin_tag,
   SYM_TAG in_sym_tag,
   const void *in_buf)
{
   DDS_BIN *out_bin, *in_bin;
   DDS_TYPE *out_type, *in_type;
   DDS_MAP *map;

   DDS_API_ENTRY("map")

   BIN_TAG_TO_BIN(out_bin, out_bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_TYPE(out_type, out_bin, out_sym_tag, DDS_API_EXIT(DDS_BAD));

   BIN_TAG_TO_BIN(in_bin, in_bin_tag, DDS_API_EXIT(DDS_BAD));

   SYM_TAG_TO_TYPE(in_type, in_bin, in_sym_tag, DDS_API_EXIT(DDS_BAD));

   map = MAP_PTR(out_bin_tag, in_bin_tag);

   /* linear search of map descriptors (depth 1 or 2 nominal) */
   while(map &&
      (map->out_object->type != out_type ||
       map->in_object->type  !=  in_type)) {
      map = map->next;
   }

   if(! map) {
      if(out_bin->state & DDS_BIN_FREEZE) {
         map = ddsOpenMap(out_bin, out_type, in_bin, in_type);
         if(! map) {
            DDS_API_EXIT(DDS_BAD)
         }
      } else {
         ddsDebug("cdds_map: map not defined for this binary combination.\n\tapplication must call 'openm' before any read, write or seek on out_bin\n\tout_bin= %s, out_type= %s\n\tin_bin= %s, in_type= %s\n",
            out_bin->name, ddsHintType(out_bin, out_type),
             in_bin->name, ddsHintType( in_bin,  in_type));
         ddsError("cdds_map: application error\n");
         DDS_API_EXIT(DDS_BAD)
      }
   }

   if(map->seam.thread) {
      map->out_object->ptr = out_buf;
      RELOC_VALUE(map->out_object);
      map->in_object->ptr = (void*)in_buf;
      RELOC_VALUE(map->in_object);

      /* evaluate mapping expressions */
      ddsEvaluate(map->seam.thread);
   }

   DDS_API_EXIT(DDS_OK)
}
