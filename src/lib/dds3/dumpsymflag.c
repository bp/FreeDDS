static char rcsid[]="$Id: dumpsymflag.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, dump DDS_SYM_FLAG

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

void ddsDumpSymFlag(
   int indent,
   DDS_SYM *sym)
{
   DDS_BOOLEAN bogus = DDS_FALSE;
   const char *prefix = "";

   if(! dds_initialized) return; /* no-op */

   if(! sym) {
      fprintf(dds_print_fp, "ddsDumpSymFlag: NULL SYM ptr\n");
      return;
   }

   fprintf(dds_print_fp, "flag= ");

   if(sym->flag & DDS_SYM_TYPE) {
      if(*prefix) bogus = DDS_TRUE;
      fprintf(dds_print_fp, "%sTYPE", prefix);
      prefix = " | ";
   }
   if(sym->flag & DDS_SYM_OBJECT) {
      if(*prefix) bogus = DDS_TRUE;
      fprintf(dds_print_fp, "%sOBJECT", prefix);
      prefix = " | ";
   }
   if(sym->flag & DDS_SYM_BLOCK) {
      if(*prefix) bogus = DDS_TRUE;
      fprintf(dds_print_fp, "%sBLOCK", prefix);
      prefix = " | ";
   }
   if(sym->flag & DDS_SYM_MEMBER) {
      if(*prefix) bogus = DDS_TRUE;
      fprintf(dds_print_fp, "%sMEMBER", prefix);
      prefix = " | ";
   }
   if(sym->flag & DDS_SYM_LABEL) {
      if(*prefix) bogus = DDS_TRUE;
      fprintf(dds_print_fp, "%sLABEL", prefix);
      prefix = " | ";
   }
   if(sym->flag & DDS_SYM_EXPR) {
      if(*prefix) bogus = DDS_TRUE;
      fprintf(dds_print_fp, "%sEXPR", prefix);
      prefix = " | ";
   }
   if(bogus || ! *prefix) {
      /* bogus, multiple flags or no flags at all were set */
      fprintf(dds_print_fp, "%s<bogus>", prefix);
      prefix = " | ";
   }


   if(sym->flag & DDS_SYM_ERROR) {
      fprintf(dds_print_fp, " | ERROR");
   }
   if(sym->flag & DDS_SYM_MULTIPLE) {
      fprintf(dds_print_fp, " | MULTIPLE");
   }
   if(sym->flag & DDS_SYM_UNDEFINED) {
      fprintf(dds_print_fp, " | UNDEFINED");
   }


   if((sym->flag & DDS_SYM_SHORT) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | SHORT");
   }
   if((sym->flag & DDS_SYM_LONG) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | LONG");
   }
   if((sym->flag & DDS_SYM_LLONG) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | LLONG");
   }
   if((sym->flag & DDS_SYM_SIGNED) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | SIGNED");
   }
   if((sym->flag & DDS_SYM_UNSIGNED) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | UNSIGNED");
   }


   if((sym->flag & DDS_SYM_VOID) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | VOID");
   }
   if((sym->flag & DDS_SYM_CHAR) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | CHAR");
   }
   if((sym->flag & DDS_SYM_INT) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | INT");
   }
   if((sym->flag & DDS_SYM_FLOAT) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | FLOAT");
   }
   if((sym->flag & DDS_SYM_DOUBLE) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | DOUBLE");
   }
   if((sym->flag & DDS_SYM_COMPLEX) && (sym->flag &
      (DDS_SYM_TYPE))) {
      fprintf(dds_print_fp, " | COMPLEX");
   }


   if(sym->flag & DDS_SYM_ARRAY) {
      fprintf(dds_print_fp, " | ARRAY");
   }
   if(sym->flag & DDS_SYM_STRUCT) {
      fprintf(dds_print_fp, " | STRUCT");
   }
   if(sym->flag & DDS_SYM_UNION) {
      fprintf(dds_print_fp, " | UNION");
   }
   if(sym->flag & DDS_SYM_ENUM) {
      fprintf(dds_print_fp, " | ENUM");
   }


   if(sym->flag & DDS_SYM_CONST) {
      fprintf(dds_print_fp, " | CONST");
   }
   if(sym->flag & DDS_SYM_VOLATILE) {
      fprintf(dds_print_fp, " | VOLATILE");
   }
   if(sym->flag & DDS_SYM_ALIGN) {
      fprintf(dds_print_fp, " | ALIGN");
   }


   if((sym->flag & DDS_SYM_AUTO) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_BLOCK))) {
      fprintf(dds_print_fp, " | AUTO");
   }
   if((sym->flag & DDS_SYM_REGISTER) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_BLOCK | DDS_SYM_EXPR))) {
      fprintf(dds_print_fp, " | REGISTER");
   }
   if((sym->flag & DDS_SYM_STATIC) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_BLOCK))) {
      fprintf(dds_print_fp, " | STATIC");
   }
   if((sym->flag & DDS_SYM_EXTERN) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_BLOCK))) {
      fprintf(dds_print_fp, " | EXTERN");
   }
   if((sym->flag & DDS_SYM_TYPEDEF) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_BLOCK))) {
      fprintf(dds_print_fp, " | TYPEDEF");
   }


   if((sym->flag & DDS_SYM_RELOC) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_EXPR | DDS_SYM_MEMBER))) {
      fprintf(dds_print_fp, " | RELOC");
   }
   if((sym->flag & DDS_SYM_LITERAL) && (sym->flag &
      (DDS_SYM_OBJECT))) {
      fprintf(dds_print_fp, " | LITERAL");
   }
   if((sym->flag & DDS_SYM_INHIBIT) && (sym->flag &
      (DDS_SYM_EXPR))) {
      fprintf(dds_print_fp, " | INHIBIT");
   }


   if((sym->flag & DDS_SYM_LVALUE) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_EXPR))) {
      fprintf(dds_print_fp, " | LVALUE");
   }


   if((sym->flag & DDS_SYM_UNCOMP) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_MEMBER))) {
      fprintf(dds_print_fp, " | UNCOMP");
   }


   if((sym->flag & DDS_SYM_CAST) && (sym->flag &
      (DDS_SYM_EXPR))) {
      fprintf(dds_print_fp, " | CAST");
   }
   if((sym->flag & DDS_SYM_COERCE) && (sym->flag &
      (DDS_SYM_EXPR))) {
      fprintf(dds_print_fp, " | COERCE");
   }


   if((sym->flag & DDS_SYM_FUNC) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_MEMBER | DDS_SYM_BLOCK | DDS_SYM_EXPR))) {
      fprintf(dds_print_fp, " | FUNC");
   }


   if((sym->flag & DDS_SYM_FIELD) && ! (sym->flag &
      (DDS_SYM_MEMBER))) {
      fprintf(dds_print_fp, " | FIELD");
   }


   if((sym->flag & DDS_SYM_DELETE) && ! (sym->flag &
      (DDS_SYM_GENUS))) {
      fprintf(dds_print_fp, " | DELETE");
   }
   if((sym->flag & DDS_SYM_REPLACE) && ! (sym->flag &
      (DDS_SYM_GENUS))) {
      fprintf(dds_print_fp, " | REPLACE");
   }
   if((sym->flag & DDS_SYM_APPEND) && ! (sym->flag &
      (DDS_SYM_GENUS))) {
      fprintf(dds_print_fp, " | APPEND");
   }
   if((sym->flag & DDS_SYM_DAPPEND) && ! (sym->flag &
      (DDS_SYM_GENUS))) {
      fprintf(dds_print_fp, " | DAPPEND");
   }
   if((sym->flag & DDS_SYM_RAPPEND) && ! (sym->flag &
      (DDS_SYM_GENUS))) {
      fprintf(dds_print_fp, " | RAPPEND");
   }


   if((sym->flag & DDS_SYM_CONST_PREP) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_EXPR))) {
      fprintf(dds_print_fp, " | CONST_PREP");
   }
   if((sym->flag & DDS_SYM_CONST_DECL) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_EXPR))) {
      fprintf(dds_print_fp, " | CONST_DECL");
   }
   if((sym->flag & DDS_SYM_CONST_INIT) && (sym->flag &
      (DDS_SYM_OBJECT | DDS_SYM_EXPR))) {
      fprintf(dds_print_fp, " | CONST_INIT");
   }


   if(sym->flag & DDS_SYM_INCOMPLETE) {
      fprintf(dds_print_fp, " | INCOMPLETE");
   }


   fprintf(dds_print_fp, ";\n");

   return;
}
