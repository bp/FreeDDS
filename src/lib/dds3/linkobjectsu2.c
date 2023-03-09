static char rcsid[]="$Id: linkobjectsu2.c 229 2011-06-02 18:53:57Z user $";
/*
RCS-t=DDS API, open mapping (compile defn) to structure member

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx July 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_opcode.h"

int ddsLinkObjectSU2(
   DDS_BIN *bin,
   DDS_OBJECT *object,
   DDS_SEAM *seam,
   const char *mode,
   const char *out_name,
   long out_bias)
{
   static DDS_STRING defn_alias = NULL_STRING;
   static DDS_STRING map_name = NULL_STRING;
   DDS_VALUE *rvalue;
   DDS_DICT *scan_dict;
   const char *dict_name = NULL;
   char scan_save;
   size_t scan_start, scan_stop;
   int rc, rank, rank2, rank_max;
   long stride;

   assert(! dds_thread && ! dds_needle && "dirty sewing machine");

   /******************************************/
   /* Search for an explicit map definition. */
   /******************************************/

   cdds_dict(bin->dict_map, "scan");
   ddsAliasName2(&defn_alias, mode, bin->alias_name, out_name);
   ddsStrCat2(&defn_alias, " ", DDS_SENTRY);

   if(! cdds_scanf(defn_alias.ptr, "") &&
      strcmp(dds_scan_name, DDS_SENTRY)) {
      /* Local map definition found, */
      /* i.e. "mode:alias.member=" */
      dict_name = bin->dict_map;
   } else {
      /* this kludge catches the default local mappings. */
      cdds_dict(strstr(bin->dict_map, "default:"), "scan");
      *(strstr(defn_alias.ptr, DDS_SENTRY) - 1) = '\0';
      if(! cdds_scanf(defn_alias.ptr, "")) {
         /* Local map definition found, */
         /* i.e. "mode:alias.member=" */
         dict_name = bin->dict_map;
      } else {
         /* Broaden scope of search for explicit map definition. */
         cdds_dict(bin->dict_map, "scan");
         ddsAliasName2(&defn_alias, mode, bin->fmt_name, out_name);

         if(! cdds_scanf(defn_alias.ptr, "")) {
            /* Global map definition found, */
            /* i.e. "mode:fmt.member=" */
            dict_name = bin->dict_map;
            /**** Not sure we really want to default mapping any sort headers - JWE May 2011 ****/
            /*  } else if(object == bin->trace.w_object) { */
         } else if(0) { 
            /* trace write mode, default sort field mapping. */
            rank_max = bin->cube.rank;
            for(rank = bin->cube.io_rank; rank <= rank_max; rank++) {
               if(! strcmp(out_name, bin->cube.axis[rank].sort_name)) {
                  /* This output member IS a sort field. */
                  if(! (bin->cube.axis[rank].flag & DDS_AXIS_SORT_SET)) {
                     /* Default the mapping for output sort field. */
                     cdds_dict(bin->dict_open, "print");
                     ddsStrCpyCat4(&defn_alias, "write:",
                        bin->alias_name, ".", out_name);

                     cdds_printf(defn_alias.ptr, " 1 + ordinal(\"%s\")",
                        bin->cube.axis[rank].name);

                     if(bin->fmt == DDS_FMT_USP && rank == 3 &&
                        ! strcmp(bin->cube.axis[rank].sort_name, "RecNum")) {
                        /* 27 July 1998 RLS, special processing to */
                        /* compute running product of higher ordinals, */
                        /* as requested by Rusty Alford, 27 July 1998. */
                        stride = bin->cube.axis[rank].size;
                        for(rank2 = rank + 1; rank2 <= rank_max; rank2++) {
                           cdds_printf("", " + %li * ordinal(\"%s\")",
                              stride, bin->cube.axis[rank2].name);
                           stride *= bin->cube.axis[rank2].size;
                        }
                     }

                     cdds_printf("", "\n");
                     cdds_dict(bin->dict_open, "scan");
                     cdds_scanf(defn_alias.ptr, "");
                     dict_name = bin->dict_open;
                  }
                  break;
               }
            }
         }
      }
   }

   /* reset scanner and declaration state */
   rvalue = NULL;
   dds_lexer = &dds_lexer_root;
   DECLARE_RESET;

   if(dict_name) {
      /* Explicit map definition found.  Compile its value */
      /* to create rvalue for map assignment expression. */
      ddsStrCpyCat5(&map_name, mode, ":", bin->alias_name, ".", out_name);

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
         cdds_dict(bin->dict_open, "print");
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
   }

   if(rvalue) {
      /* This output member IS mapped. */
      ddsMapAssign(map_name.ptr, bin, object, out_name,
         rvalue, object, seam);
   }

   return DDS_OK;
}
