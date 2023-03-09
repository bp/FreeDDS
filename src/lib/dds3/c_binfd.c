static char rcsid[]="$Id: c_binfd.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, binary file descriptor, given a bin_tag

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_binfd(
   BIN_TAG bin_tag)
{
   DDS_BIN *bin;
   int fd;

   DDS_API_ENTRY("binfd")

   BIN_TAG_TO_BIN(bin, bin_tag, DDS_API_EXIT(DDS_BAD));

   fd = gfileno(bin->media->fp);
   bin->media->fp->cfd.tmp_close_ok = GIO_FALSE;

   DDS_API_EXIT(fd)
}
