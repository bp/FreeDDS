static char rcsid[]="$Id: c_lseek.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, long seek position within binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

long cdds_lseek(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   long offset,
   int whence)
{
   HOST_INT6_T offset8;

   DDS_API_ENTRY("lseek")

   offset8 = cdds_lseek8(bin_tag, sym_tag, (HOST_INT6_T)offset, whence);

   offset = offset8;

   if(offset == offset8) {
      DDS_API_EXIT(offset);
   }

   DDS_API_EXIT(DDS_BAD);
}
