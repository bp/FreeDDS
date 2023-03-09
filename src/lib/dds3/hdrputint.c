static char rcsid[]="$Id: hdrputint.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, put header int value into binary buffer

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsHdrPutInt(
   const int *bin_hdr,
   const char *name,
   char *buf,
   DDS_BLOCK *block)
{
   DDS_MEMBER *member;
   DDS_TYPE *type;
   SYM_TAG tag;
#if defined(_WIN32) || defined(MINGW)
   size_t *bias_ptr, bias = 0;
#else
   long *bias_ptr, bias = 0;
#endif
   member = ddsSymLookupMem(dds_symbol, name, block, &bias);
   if(! member) return;
   
   type = member->type;
   buf += bias;

   CON_NAVV(type->ptype, DDS_CTYPE_INT, buf, bin_hdr, type->count);

   SYM_TO_SYM_TAG(tag, &dds_compile->sym_tag, member)
   SYM_TAG_TO_BIAS(bias_ptr, dds_compile, tag, return;)      
   *bias_ptr = bias;     
			
   return;
}
