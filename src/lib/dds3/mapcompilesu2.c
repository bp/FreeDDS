static char rcsid[]="$Id: mapcompilesu2.c 138 2010-02-05 17:21:11Z user $";
/*
RCS-t=DDS API, compile map for struct members

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx July 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

int ddsMapCompileSU2(
   DDS_MAP *map,
   const char *out_name,
   long out_bias,
   DDS_MEMBER *out_member,
   DDS_BOOLEAN map_default)
{
   static DDS_STRING defn_alias1 = NULL_STRING;
   static DDS_STRING defn_alias2 = NULL_STRING;
   static DDS_STRING map_name = NULL_STRING;
   DDS_BIN *out_bin = map->out_bin, *in_bin = map->in_bin;
   DDS_TYPE *in_type = map->in_object->type;
   DDS_PTYPE stype1, stype2;
   DDS_MEMBER *in_member;
   DDS_VALUE *rvalue;
   DDS_OBJECT *in_object;
   DDS_DICT *scan_dict;
   DDS_EXPR4 *expr4;
   DDS_EXPRC *exprc;
   DDS_BOOLEAN optimize;
#if defined(_WIN32) || defined(MINGW)
   size_t in_bias;
   long stroke, stroke2;
#else
   long in_bias, stroke, stroke2;
#endif
   const char *dict_name = NULL;
   char scan_save;
   size_t scan_start, scan_stop;
   int rank, rank_max;
   int rc, status = DDS_OK;

   assert(! dds_thread && ! dds_needle && "dirty sewing machine");

   exprc = (DDS_EXPRC*)map->seam.needle;

   /* Optimize if the following conditions are met */
   if(map->seam.needle && 
      /* previous expression is byte copy. */
      map->seam.needle->opcode == DDS_COPY_OP &&
      /* previous DDS_COPY_OP destination is contigous. */
      out_bias == exprc->lhs_bias + exprc->arg3.value.idx &&
      /* previous RHS is a structure member dereference. */
      ((DDS_EXPR*)exprc->arg2.track)->opcode == DDS_STRUCT_MEMBER_OP &&
      /* previous structure object is the map input buffer. */
      ((DDS_EXPR2*)exprc->arg2.track)->arg1.track == (DDS_VALUE*)map->in_object) {
      optimize = DDS_TRUE;
   } else {
      optimize = DDS_FALSE;
   }

   /**************************************/
   /* Seti[ search mapping definitions   */
   /**************************************/

   ddsAliasName3(&defn_alias1, "map", in_bin->alias_name,
      out_bin->alias_name, out_name);
   ddsStrCat2(&defn_alias1, " ", DDS_SENTRY);

   ddsAliasName3(&defn_alias2, "map", in_bin->fmt_name,
      out_bin->fmt_name, out_name);
   ddsStrCat2(&defn_alias2, " ", DDS_SENTRY);

   /******************************************/
   /* Search for an explicit map definition. */
   /******************************************/

   /* Local map definition found, */
   cdds_dict(out_bin->dict_map, "scan");
   if (! cdds_scanf(defn_alias1.ptr, "") &&
      strcmp(dds_scan_name, DDS_SENTRY)) {
      /* found "map:in_alias:out_alias.member=" */
      dict_name = out_bin->dict_map;
   } else if (! cdds_scanf(defn_alias2.ptr, "") &&
      strcmp(dds_scan_name, DDS_SENTRY)) {
      /* found "map:in_fmt:out_fmt.member=" */
      dict_name = out_bin->dict_map;
   } else {
      /* Search for the default mappings. */
      *(strstr(defn_alias1.ptr, DDS_SENTRY) - 1) = '\0';
      *(strstr(defn_alias2.ptr, DDS_SENTRY) - 1) = '\0';
      cdds_dict(strstr(out_bin->dict_map, "default:"), "scan");
      if (! cdds_scanf(defn_alias1.ptr, "")) {
         /* i.e. "map:in_alias:out_alias.member=" */
         dict_name = out_bin->dict_map;
      } else if(! cdds_scanf(defn_alias2.ptr, "")) {
         /* i.e. "map:in_fmt:out_fmt.member=" */
         dict_name = out_bin->dict_map;
      } else {
         cdds_dict(out_bin->dict_map, "scan"); 
      }
   }

   /* Select the symbol table for the input binary. */
   dds_compile = in_bin;
   dds_symbol = &in_bin->symbol;
   dds_block = map->in_object->block;

   /* reset scanner and declaration state */
   rvalue = NULL;
   dds_lexer = &dds_lexer_root;
   DECLARE_RESET;

   if(dict_name) {
      /* Explicit map definition found.  Compile its value */
      /* to create rvalue for map assignment expression. */
      ddsStrCpyCat6(&map_name, "map:", in_bin->alias_name, ":",
         out_bin->alias_name, ".", out_name);

      dds_lex_ss = DDS_LEX_MAP;
      dds_hist = dict_name;

      ddsLexStr(DDS_TRUE, dds_scan_dict->name, dds_scan_name,
         dds_scan_token.next, dds_scan_dict->buf.ptr + dds_scan_newer->start,
         dds_scan_defn->line);

      if(dds_scan_dict->state & DDS_DICT_MAP) {
         /* copy map defn from special dictionary to output */
         scan_dict = dds_scan_dict;
         scan_start = dds_scan_value_bias;
         scan_stop = dds_scan_newer->start;
         cdds_dict(map->out_bin->dict_open, "print");
         if(dds_print_first && ! (dds_print_first->state & DDS_DICT_LOAD)) {
            /* output dictionary was created or truncated (not loaded) */
            scan_save = scan_dict->buf.ptr[scan_stop];
            scan_dict->buf.ptr[scan_stop] = '\0';
            cdds_printv(map_name.ptr, scan_dict->buf.ptr + scan_start);
            scan_dict->buf.ptr[scan_stop] = scan_save;
         }
      }

      /* Compile map expression (defn value). */
      rc = ddsYYparse(&rvalue);

      dds_hist = NULL;

      if(rc != DDS_OK) return DDS_BAD;

      if(rvalue) {
         /* "DDS_BAD" here simply means something was mapped. */
         /* This propagates up, inhibiting duplicate union maps. */
         status = DDS_BAD;

         if(optimize) {
            /* The previous expression might be optimizable and  */
            /* the parser did provide a RHS value for mapping. */
            stype1 = dds_prime[out_member->type->ptype].stype;
            stype2 = dds_prime[rvalue->type->ptype].stype;

            if(dds_copy_op[stype1 - DDS_STYPE_MIN][stype2 - DDS_STYPE_MIN]
               && (rvalue->flag & DDS_SYM_EXPR)) {
               /* This assignment is a simple byte copy. */
               expr4 = (DDS_EXPR4*)rvalue;

               if(expr4->opcode == DDS_STRUCT_MEMBER_OP) {
                  /* byte offset to the struct member */
                  in_bias = expr4->arg2.value.idx;
                  if(in_bias == exprc->rhs_bias + exprc->arg3.value.idx) {
                     /*****************************************************/
                     /* Its origin IS contigous with previous expression. */
                     /*                                                   */
                     /* Optimize by simply copying more bytes.            */
                     /*****************************************************/
                     exprc->arg3.value.idx += rvalue->type->stroke;

                     /* Inhibit subsequent map processing. */
/*    Note: the RHS value may still be relocated, although its not used. */
/*    This creates some overhead, but hopefully its relatively small. */
                     rvalue = NULL;
                  }
               }
            }
         }
      }
   } else if(map_default) {
      /* Explicit map definition NOT found and defaulting is enabled. */
      /* Check for the default, i.e. identical names. */
      ddsStrCpyCat6(&map_name, "map:", in_bin->alias_name, ":",
         out_bin->alias_name, ".", out_name);
      if(in_type->flag & (DDS_SYM_STRUCT | DDS_SYM_UNION)) {
         /* Input buffer type is struct/union. */
         in_member = ddsSymLookupMem(&in_bin->symbol, out_name,
            in_type->block, &in_bias);
         if(in_member) {
            /* identical member name found. */
            if(in_member->type->ptype != DDS_CTYPE_VOID
               && in_member->type->stroke) {
               /* input member type is NOT void */
               /* and array dimensions are not zero. */
               /* Default mapping found (identical member name). */

               /* "DDS_BAD" here simply means something was mapped. */
               /* This propagates up, inhibiting duplicate union maps. */
               status = DDS_BAD;

               stype1 = dds_prime[out_member->type->ptype].stype;
               stype2 = dds_prime[in_member->type->ptype].stype;

               if(optimize &&
                  dds_copy_op[stype1 - DDS_STYPE_MIN][stype2 - DDS_STYPE_MIN]
                  && in_bias == exprc->rhs_bias + exprc->arg3.value.idx) {
                  /*****************************************************/
                  /* The previous expression might be optimizable.     */
                  /* This assignment is a simple byte copy and         */
                  /* its origin IS contigous with previous expression. */
                  /*                                                   */
                  /* Optimize by simply copying more bytes.            */
                  /*****************************************************/
                  stroke = out_member->type->stroke;
                  stroke2 = in_member->type->stroke;
                  if (stroke2 < stroke) stroke = stroke2;
                  exprc->arg3.value.idx += stroke;
               } else {
                  /* Create rvalue expression (input member reference). */
                  SCAMMER_PUSH("<default>", map_name.ptr);
                  rvalue = ddsSymExpr2C(NULL, DDS_STRUCT_MEMBER_OP,
                     (DDS_VALUE*)map->in_object, out_name);
                  SCAMMER_POP;
               }
            }
         }
      } else {
         /* Input buffer type is NOT struct/union. */
         in_object = LOOKUP_OBJECT(&in_bin->symbol,
            out_member->name, in_type->block);
         if(in_object) {
            /* Default mapping found (identical object name). */
            /* Create rexpr for map assignment expression. */
            ddsPanic("ddsMapStruct: strange default input type\n");
         }
      }
   }

   if(rvalue) {
      /* This output member IS mapped. */
      ddsMapAssign(map_name.ptr, out_bin, map->out_object, out_name,
         rvalue, map->in_object, &map->seam);
   }

   if(status == DDS_BAD) {
      /* "DDS_BAD" here simply means something was mapped. */
      rank_max = out_bin->cube.rank;
      for(rank = out_bin->cube.io_rank; rank <= rank_max; rank++) {
         if(! strcmp(out_name, out_bin->cube.axis[rank].sort_name)) {
            /* This output member IS a sort field that is being mapped. */
            out_bin->cube.axis[rank].flag |= DDS_AXIS_SORT_SET;
            break;
         }
      }
   }

   return status;
}
