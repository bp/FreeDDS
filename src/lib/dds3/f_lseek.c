static char rcsid[]="$Id: f_lseek.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, logical seek on binary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_FTN_INT
HOST_FTN_FUNC(fdds_lseek, FDDS_LSEEK)(
   HOST_FTN_INT *bin_tag,
   HOST_FTN_INT *sym_tag,
   HOST_FTN_INT *offset,
   HOST_FTN_INT *whence)
{
   HOST_INT6_T offset8;
   HOST_FTN_INT foffset;

   DDS_API_INIT;

   offset8 = cdds_lseek8(*bin_tag, *sym_tag, (HOST_INT6_T)*offset, *whence);

   foffset = offset8;

   if(foffset == offset8) {
      return foffset;
   }

   return DDS_BAD;
}
