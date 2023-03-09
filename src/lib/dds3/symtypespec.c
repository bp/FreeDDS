static char rcsid[]="$Id: symtypespec.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create DDS_TYPE "specifier" in symbol table.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_TYPE * ddsSymTypeSpec(
   DDS_SYM_FLAG flag,
   DDS_SYM_FLAG mask)
{
   DDS_TYPE *type, *ptype_type;
   DDS_PTYPE ptype;
   DDS_SYM_FLAG spec_mask = (DDS_SYM_FLAG)(DDS_SYM_SPEC & ~ mask);
   DDS_SYM_FLAG type_flag;

   if(dds_declare->type) {
      /* use existing type */
      type = dds_declare->type;
      if((flag && (type->flag & DDS_SYM_ASUE)) || (type->type)) {
         /* example: struct{int j;} integer2 x; */
         /* example: int * integer2 j; */
         type->flag |= DDS_SYM_ERROR;
         ddsYYError("expected an identifier, not a type specifier\n");
         return type;
      } else if(type->flag & spec_mask) {
         /* example: int float j; */
         ddsYYError("invalid combination of type specifiers\n");
         type->flag |= DDS_SYM_ERROR;
         return type;
      } else {
         type->flag |= flag;
      }
   } else {
      /* create and initialize DDS_TYPE descriptor */
      type = PUSH_TYPE(dds_symbol, NULL, flag,
         dds_block, NULL);
      dds_declare->more = dds_declare->type = type;
   }

   type_flag = type->flag;

   if(type_flag & DDS_SYM_SPEC) {
      /* lex context, want IDENTIFIER_TOKEN (not existing object) */
      /* Required when type specifier has been seen, i.e. */
      /*    type BASE, MODIFY, SUE, or typedef name. */
      dds_declare->flag |= DDS_SYM_IDENT;
   } else {
      /* lex context, want existing object (not IDENTIFIER_TOKEN) */
      /* Required when only a type qualifier has been seen, i.e. */
      /*    "const", "volatile", or "align". */
      dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT;
   }

   /* decode accumulated flags (variations and type genus) */
   if(type_flag & DDS_SYM_VOID) {
      ptype = DDS_CTYPE_VOID;
   } else if(type_flag & DDS_SYM_CHAR) {
      if(type_flag & DDS_SYM_SIGNED) {
         ptype = DDS_CTYPE_SCHAR;
      } else if(type_flag & DDS_SYM_UNSIGNED) {
         ptype = DDS_CTYPE_UCHAR;
      } else {
         ptype = DDS_CTYPE_CHAR;
      }
   } else if(type_flag & DDS_SYM_UNSIGNED) {
      if(type_flag & DDS_SYM_SHORT) {
         ptype = DDS_CTYPE_USHORT;
      } else if(type_flag & DDS_SYM_LLONG) {
         ptype = DDS_CTYPE_ULLONG;
      } else if(type_flag & DDS_SYM_LONG) {
         ptype = DDS_CTYPE_ULONG;
      } else {
         ptype = DDS_CTYPE_UINT;
      }
   } else if((type_flag & DDS_SYM_SIGNED) ||
             (type_flag & DDS_SYM_INT) ||
             (type_flag & DDS_SYM_SHORT)) {
      if(type_flag & DDS_SYM_SHORT) {
         ptype = DDS_CTYPE_SHORT;
      } else if(type_flag & DDS_SYM_LLONG) {
         ptype = DDS_CTYPE_LLONG;
      } else if(type_flag & DDS_SYM_LONG) {
         ptype = DDS_CTYPE_LONG;
      } else {
         ptype = DDS_CTYPE_INT;
      }
   } else if(type_flag & DDS_SYM_FLOAT) {
      if(type_flag & DDS_SYM_COMPLEX) {
         ptype = DDS_CTYPE_CPX;
      } else {
         ptype = DDS_CTYPE_FLOAT;
      }
   } else if(type_flag & DDS_SYM_DOUBLE) {
      if(type_flag & DDS_SYM_LONG) {
         if(type_flag & DDS_SYM_COMPLEX) {
            ptype = DDS_CTYPE_LDBLCPX;
         } else {
            ptype = DDS_CTYPE_LDOUBLE;
         }
      } else if(type_flag & DDS_SYM_COMPLEX) {
         ptype = DDS_CTYPE_DBLCPX;
      } else {
         ptype = DDS_CTYPE_DOUBLE;
      }
   } else if(type_flag & DDS_SYM_COMPLEX) {
      ptype = DDS_CTYPE_CPX;
   } else {
      ptype = DDS_CTYPE_INT;
   }

   type->name = dds_prime[ptype].name;

   type->ptype = ptype;

   ptype_type = dds_prime[ptype].type;

   type->type = ptype_type->type;
   type->align = ptype_type->align;
   type->prec = ptype_type->prec;
   type->count = ptype_type->count;
   type->stroke = ptype_type->stroke;

   return type;
}
