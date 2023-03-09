static char rcsid[]="$Id: seekheader.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, line header seek within binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

HOST_INT6_T ddsSeekHeader(
   DDS_BIN *bin,
   DDS_TYPE *type,
   HOST_INT6_T offset,
   int whence)
{
   ddsError("seek on line header\n\t%s, LineTrc= %lu\n",
      bin->media->title.ptr, (unsigned long)bin->LineTrc);

   return DDS_BAD;
}
