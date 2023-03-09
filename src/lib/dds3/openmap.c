static char rcsid[]="$Id: openmap.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, open mapping to output binary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx July 1996           ***/
/**********************************************/

   /**********************************************************************/
   /* OVERVIEW:                                                          */
   /*    Map definitions are retrieved and their expressions compiled.   */
   /*    Mapping expressions are processed in output field offset order. */
   /*    Expressions that are effectively "byte copy" are identified.    */
   /*    Linear sequences of byte copies are merged into one expression. */
   /*    Merged "byte copy" is significantly faster, when mapping        */
   /*    a list of scalar members in a struct (trace header fields).     */
   /*    Similar optimization is applied to assignment sequences that    */
   /*    convert between the same pair of types (int to float, etc.)     */
   /*    Expressions in the output symbol table are evaluated when the   */
   /*    cdds_map function is called (perhaps via cdds_readm or writem). */
   /*                                                                    */
   /* FUNCTIONS: calling hierarchy of map specific processing.           */
   /*    cdds_openm                                                      */
   /*      ddsMapCompile          process output in offset order         */
   /*        ddsMapStruct         iff output ptype is DDS_STYPE_STRUCT   */
   /*          ddsMapAssign       synthesize assignment                  */
   /*            ddsMapOptimize   optimize assignment                    */
   /*        ddsMapUnion          iff output ptype is DDS_STYPE_UNION    */
   /*        ddsMapVect           iff output ptype is "vector"           */
   /**********************************************************************/

/***************************************************************************/
/*  Note: cdds_openm needs to access dictionaries "remembered" by out_bin. */
/*  These dictionaries might have been closed by the application :^(.      */
/*                                                                         */
/*  This is a fatal error, if the dictionaries are piped or were accessed  */
/*  via a file descriptor (dictionary contents are lost).                  */
/*  RATIONAL ERROR MESSAGES NEED TO IMPLEMENTED.                           */
/*                                                                         */
/*  If the dictionaries are regular files, cdds_openm must preserve their  */
/*  contents by initially selecting them in scan mode.                     */
/***************************************************************************/

#include "dds_sys.h"

DDS_MAP * ddsOpenMap(
   DDS_BIN *out_bin,
   DDS_TYPE *out_type,
   DDS_BIN *in_bin,
   DDS_TYPE *in_type)
{
   static DDS_STRING map_name = NULL_STRING;
   static int serial = 0;
   BIN_TAG out_bin_tag, in_bin_tag;
   DDS_MEDIA *media;
   DDS_MAP *map;
   char tmp[32];
   const char *name;

   assert(out_bin);

   if(! out_type) {
      /* default out type */
      out_type = *(DDS_TYPE**)out_bin->sym_tag.ptr;
   }

   if(! in_bin) {
      /* default in_bin */
      if(in_type) {
         ddsDebug("ddsOpenMap: in_type, but no in_bin\n");
         return NULL;
      }
      in_bin = out_bin; /* default for default */
      for(media = out_bin->media->right; media; media = media->right) {
         if(media->bin) {
            /* default in_bin to the binary prior to out_bin */
            in_bin = media->bin;
            break;
         }
      }
   }

   if(! in_type) {
      /* default in_type */
      in_type = *(DDS_TYPE**)in_bin->sym_tag.ptr;
   }

   if((out_type->flag | in_type->flag) & DDS_SYM_ERROR) {
      ddsVerbose("ddsOpenMap: bad input and/or output types\n");
      return NULL;
   }

   out_bin_tag = out_bin->tag;
   in_bin_tag = in_bin->tag;

   /* check for an existing map descriptor. */
   map = MAP_PTR(out_bin_tag, in_bin_tag);
 
   /* linear search of map descriptors (depth 1 or 2 nominal) */
   while(map &&
      (map->out_object->type != out_type ||
       map->in_object->type  !=  in_type)) {
      map = map->next;
   }

   if(map) {
      ddsDebug("ddsOpenMap: map opened more than once\n\tout_bin= %s, out_type= %s\n\tin_bin= %s, in_type= %s\n",
         out_bin->name, ddsHintType(out_bin, out_type),
          in_bin->name, ddsHintType( in_bin,  in_type));
      return NULL;
   }

   /*************************/
   /* setup for map compile */
   /*************************/

   /* unique serial number for this map */
   sprintf(tmp, "%i", ++serial);

   /* Create map descriptor for this unique combination of */
   /* output (bin and type) and input (bin and type). */
   map = (DDS_MAP*)ddsZalloc(sizeof(DDS_MAP));
   map->next = MAP_PTR(out_bin_tag, in_bin_tag);
   map->out_bin = out_bin;
   map->in_bin = in_bin;

   /* Select the symbol table for the output binary. */
   dds_compile = out_bin;
   dds_symbol = &out_bin->symbol;
   dds_block = out_bin->block_new;
 
   /* create unique block for output symbols. */
   ddsStrCpyCat6(&map_name, "map:", in_bin->alias_name,
      ":", out_bin->alias_name, ":", tmp);
   name = ddsTablePushStr(dds_symbol, map_name.ptr);
   ddsSymBlockBegin(name, (DDS_SYM_FLAG)0, NULL);

   /* create object for output buffers */
   map->out_object = PUSH_OBJECT(dds_symbol, "<out_object>",
      (DDS_SYM_FLAG)(DDS_SYM_REGISTER | DDS_SYM_RELOC | DDS_SYM_LVALUE),
      dds_block, out_type);

   /* Select the symbol table for the input binary. */
   dds_compile = in_bin;
   dds_symbol = &in_bin->symbol;
   dds_block = in_bin->block_new;
 
   /* create unique block for input symbols. */
   ddsStrCpyCat6(&map_name, "map:", in_bin->alias_name,
      ":", out_bin->alias_name, ":", tmp);
   name = ddsTablePushStr(dds_symbol, map_name.ptr);
   ddsSymBlockBegin(name, (DDS_SYM_FLAG)0, NULL);

   /* create object for input buffers */
   map->in_object  = PUSH_OBJECT(dds_symbol, "<in_object>",
      (DDS_SYM_FLAG)(DDS_SYM_REGISTER | DDS_SYM_RELOC),
      dds_block, in_type);

   dds_map_value = (DDS_VALUE*)map->in_object;

   /***********************************************/
   /* compile map definitions into symbol tables. */
   /***********************************************/

   ddsMapCompile(map);

   /***************************/
   /* cleanup for map compile */
   /***************************/

   /* Select the symbol table for the output binary. */
   dds_compile = out_bin;
   dds_symbol = &out_bin->symbol;
   dds_block = map->out_object->block;
   DECLARE_INNER; /* kludge for ddsSymBlockEnd */

   /* exit the block for output symbols. */
   ddsSymBlockEnd();

   /* Select the symbol table for the input binary. */
   dds_compile = in_bin;
   dds_symbol = &in_bin->symbol;
   dds_block = map->in_object->block;
   DECLARE_INNER; /* kludge for ddsSymBlockEnd */

   /* exit the block for input symbols. */
   ddsSymBlockEnd();

   MAP_PTR(out_bin_tag, in_bin_tag) = map;

   dds_map_value = NULL;
   dds_state &= (DDS_SYM_FLAG) ~ (DDS_YACC_MAP | DDS_YACC_IO);

   return map;
}
