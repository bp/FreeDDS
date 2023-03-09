static char rcsid[]="$Id: compilefmt.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open and compile format for binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"


int ddsCompileFmt(
   DDS_BIN *bin,
   DDS_BOOLEAN create)
{
   static DDS_STRING root_defn = NULL_STRING;
   int parse_rc;

   if(cdds_dict(bin->dict_fmt, "scan")) {
      ddsError("can't open old format, dict= %s\n", bin->dict_fmt);
      return DDS_BAD;
   }

   if(bin->state & DDS_BIN_OPEN_CREATE) {
      /* create new data and associated format */
      ddsStrCpyCat6(&root_defn, "fmt:*:", bin->fmt_name,
         " fmt:", bin->old_format, ":", bin->fmt_name);
      dds_state = (DDS_STATE)(DDS_YACC_CREATE |
         (dds_state & ~ (DDS_YACC_MAP | DDS_YACC_IO)));
      if(cdds_scanf(root_defn.ptr, "")) {
         /* Recipe does NOT exist for creating the new format. */
         /* Assume the new format is identical to the old. */
         ddsStrCpyCat2(&root_defn, "fmt:", bin->fmt_name);
         if(cdds_scanf(root_defn.ptr, "")) {
            ddsError("can't open new format, defn= %s\n\tdict= %s\n",
               root_defn.ptr, bin->dict_fmt);
            return DDS_BAD;
         }
      }
   } else {
      /* access existing data and predetermined format */
      ddsStrCpyCat2(&root_defn, "fmt:", bin->fmt_name);
      dds_state &= (DDS_STATE) ~ (DDS_YACC_MAP | DDS_YACC_IO |
         DDS_YACC_CREATE);
      if(cdds_scanf(root_defn.ptr, "")) {
         ddsError("can't open old format, defn= %s\n\tdict= %s\n",
            root_defn.ptr, bin->dict_fmt);
         return DDS_BAD;
      }
   }


   /**********************************************/
   /* compile format definitions for binary data */
   /**********************************************/

   dds_hist = bin->dict_fmt;
   dds_lex_ss = DDS_LEX_TRANSLATE;
   dds_thread = NULL;
   dds_needle = NULL;

   dds_lexer = &dds_lexer_root;
   DECLARE_RESET;

   ddsLexStr(DDS_TRUE, dds_scan_dict->name, dds_scan_name, dds_scan_token.next,
      dds_scan_dict->buf.ptr + dds_scan_newer->start,
         dds_scan_defn->line);

   parse_rc = ddsYYparse(NULL);

   if(parse_rc) {
      ddsError("fatal problem in format description\n\t%s\n",
         bin->dict_fmt);
      return parse_rc;
   }

   if(dds_thread) {
      /* evaluate dangling thread after compilation */
      ddsEvaluate(dds_thread);
   }

   return DDS_OK;
}
