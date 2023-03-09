static char rcsid[]="$Id: linkmember.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, find member and resolve it's type

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsLinkMember(
   DDS_MEMBER_REF *ref,
   DDS_BLOCK *block,
   const char *name)
{
   DDS_MEMBER *member;
   SYM_TAG tag;
#if defined(_WIN32) || defined(MINGW)
   size_t *bias;
#else
   long *bias;
#endif
   int rc = DDS_OK;

   member = ddsSymLookupMem(dds_symbol, name, block, &ref->offset);
   if(member) {
      SYM_TO_SYM_TAG(tag, &dds_compile->sym_tag, member)
      ref->member = member;
      ref->type = member->type;
      SYM_TAG_TO_BIAS(bias, dds_compile, tag, return rc; ) 
      *bias = ref->offset;
   } else {
      rc = DDS_BAD;
      ref->member = dds_error_member;
      ref->type = dds_error_type;
   }

   return rc;
}
