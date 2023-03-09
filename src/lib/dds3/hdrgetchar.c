static char rcsid[]="$Id: hdrgetchar.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, get header char value from dictionary and/or binary buffer

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsHdrGetChar(
   char *dict_hdr,
   char *bin_hdr,
   const char *name,
   const char *defn,
   const char *buf,
   DDS_BLOCK *block)
{
   DDS_MEMBER *member;
   DDS_TYPE *type;
   SYM_TAG tag;
   int n;
   size_t max, prec, count; 
#if defined(_WIN32) || defined(MINGW)
   size_t *bias_ptr, bias = 0;
#else
   long *bias_ptr, bias = 0;
#endif
   const char *value;

   member = ddsSymLookupMem(dds_symbol, name, block, &bias);
   if(! member) return;
   
   type = member->type;
   buf += bias;

   prec  = type->prec;
   count = type->count;

   if(! cdds_scanf(defn, "")) {
      for(n = 0; n < count; n++) {
         if(0 > cdds_scant("", &value)) break;
         max = strlen(value);
         if(max < prec) {
            memcpy(dict_hdr + n * prec, value, max);
            memset(dict_hdr + n * prec + max, ' ', prec - max);
         } else {
            memcpy(dict_hdr + n * prec, value, prec);
         }
      }
      if(n < count)
         memset((void*)(dict_hdr + n * prec), ' ', (count - n) * prec);
      /* dictionary overrides binary buffer */
      memcpy((void*)bin_hdr, (const void*)dict_hdr, count * prec);
   } else {
      memset((void*)dict_hdr, ' ', count * prec);
      /* retain original value in binary buffer */
      CON_AAVV(HOST_CHAR_DDS, type->ptype, bin_hdr, buf, count * prec);
      for(n=0;n<count*prec;n++) {
         if (bin_hdr[n] == '\\' || !isprint(bin_hdr[n])) {
            bin_hdr[n] = '~';
         }
      }
   }

   SYM_TO_SYM_TAG(tag, &dds_compile->sym_tag, member)
   SYM_TAG_TO_BIAS(bias_ptr, dds_compile, tag, return;)
   *bias_ptr = bias;

   return;
}
