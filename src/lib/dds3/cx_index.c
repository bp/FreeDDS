static char rcsid[]="$Id: cx_index.c 168 2010-08-04 17:38:18Z user $";
/*
RCS-t=DDS API, convenience to return buffer index (zero based) to a
               header
*/

/***************************************/
/***   Copyright (C) 2006            ***/
/***   BP America, Houston, TX       ***/
/***   Jerry Ehlers, August, 2006    ***/
/***************************************/

#include "dds_sys.h"


int cddx_index(
   BIN_TAG bin_tag,
   const char *name,
   int prime) 
{
   SYM_TAG tag;
   int offset;


   int status;
   DDS_BIN *bin;

   DDS_API_ENTRY("xindex")
  
   if (bin_tag < 0 || bin_tag >= dds_bin_max) {
      cdds_prterr("(cddx_index)Illegal bin_tag=%d\n",bin_tag);
      DDS_API_EXIT(EOF);
   }
   if (!name || *name=='\0') {
      cdds_prterr("(cddx_index) No name provide\n");
      DDS_API_EXIT(EOF);
   }
   VALID_PRIME(prime, DDS_API_EXIT(DDS_BAD))
   
   tag = cdds_member(bin_tag, 0, name);
   if (tag < 0) {
      DDS_API_EXIT(EOF);
   }

   offset = cdds_index(bin_tag, tag, prime);

   DDS_API_EXIT(offset);
}
