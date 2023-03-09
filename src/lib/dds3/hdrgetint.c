static char rcsid[]="$Id: hdrgetint.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, get header int value from dictionary and/or binary buffer

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsHdrGetInt(
   int *dict_hdr,
   int *bin_hdr,
   const char *name,
   const char *defn,
   const char *buf,
   DDS_BLOCK *block)
{
   int n;
   size_t count;
#if defined(_WIN32) || defined(MINGW)
   size_t *bias_ptr, bias = 0;
#else
   long *bias_ptr, bias = 0;
#endif
   SYM_TAG tag;
   DDS_MEMBER *member;
   DDS_TYPE *type;

   member = ddsSymLookupMem(dds_symbol, name, block, &bias);
   if(! member) return;
   
   type = member->type;
   buf += bias;

   count = type->count;

   if(! cdds_scanf(defn, "")) {
      for(n = 0; n < count; n++) {
         if(1 != cdds_scanf("", "%i", dict_hdr + n)) break;
      }
      if(n < count)
         memset((void*)(dict_hdr + n), '\0', (count - n) * sizeof(int));
      /* dictionary overrides binary buffer */
      memcpy((void*)bin_hdr, (const void*)dict_hdr, count * sizeof(int));
   } else {
      memset((void*)dict_hdr, '\0', count * sizeof(int));
      /* retain original value in binary buffer */
      CON_ANVV(DDS_CTYPE_INT, type->ptype, bin_hdr, buf, count);
   }
 
   SYM_TO_SYM_TAG(tag, &dds_compile->sym_tag, member) 
   SYM_TAG_TO_BIAS(bias_ptr, dds_compile, tag, return;)
   *bias_ptr = bias; 

   return;
}
