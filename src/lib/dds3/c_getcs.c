static char rcsid[]="$Id: c_getcs.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, get "C" string (null terminated)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_getcs(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const void *buf,
   char *array,
   int count)
{
   DDS_BIN *bin;
   DDS_TYPE *type;
   DDS_SYM *sym;
   DDS_CON_STATUS status;
   size_t prec, num, nbytes, offset;

   DDS_API_ENTRY("getcs")

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(0));

   NON_NULL(buf, bin, DDS_API_EXIT(0));

   NON_NULL(array, bin, DDS_API_EXIT(0));

   NON_NEGATIVE(count, bin, DDS_API_EXIT(0));

   SYM_TAG_TO_SYM(sym, bin, sym_tag, DDS_API_EXIT(0));

   if(sym->type) {
      type = sym->type;
      prec = dds_prime[type->ptype].prec;
      if(prec) {
         num = type->stroke / prec;
         if(num > count) {
            num = count;
            nbytes = 0;
         } else {
            nbytes = count - num;
         }
         offset = (sym->flag & DDS_SYM_MEMBER) ?
            ((DDS_MEMBER*)sym)->offset : 0;
         status = CON_ANVV(DDS_CTYPE_CHAR, type->ptype,
            array, (char*)buf + offset, num);
         if(status == DDS_CON_OK)
            ((char*)array)[num] = '\0';
         else
            num = 0;
      } else {
         ddsDebug("cdds_getcs: invalid type\n\tbin=%s\n\tsymbol= %s, type= %s\n",
            bin->name, sym->name ? sym->name : "<anonymous>",
            ddsHintType(bin, type));
         num = 0;
      }
   } else {
      ddsDebug("cdds_getcs: type not defined\n\tbin=%s\n\tsymbol= %s\n",
         bin->name, sym->name ? sym->name : "<anonymous>");
      num = 0;
   }
   DDS_API_EXIT(num);
}

