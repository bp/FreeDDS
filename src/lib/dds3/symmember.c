static char rcsid[]="$Id: symmember.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create struct/union DDS_MEMBER in symbol table

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_MEMBER * ddsSymMember(
   const char *name)
{
   DDS_MEMBER *member;
   DDS_TYPE *type;
   DDS_SYM_FLAG flag;

   type = dds_declare->type;
   flag = (DDS_SYM_FLAG)(type->flag &
      (DDS_SYM_ERROR | DDS_SYM_ASUE | DDS_SYM_QUAL));

   member = PUSH_MEMBER(dds_symbol, name, flag,
      dds_block, type);

   /* lex context, want existing object (not IDENTIFIER_TOKEN) */
   /* Required for constant expressions after ":", i.e. */
   /*    struct {int member_name : enum_name; ...} foo; */
   dds_declare->flag &= (DDS_SYM_FLAG) ~ DDS_SYM_IDENT;

   if(type->flag & DDS_SYM_INCOMPLETE) {
      if(type->count == SIZE_T_MAX) {
         ddsYYError("struct/union member '%s' is incomplete (array [])\n",
            name);
      } else {
         ddsYYError("struct/union member '%s' is incomplete\n",
            name);
      }
      member->flag |= (DDS_SYM_FLAG)(DDS_SYM_ERROR);
   }

   return member;
}
