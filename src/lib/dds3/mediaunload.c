static char rcsid[]="$Id: mediaunload.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, unload media from drive

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_magtape.h"

int ddsMediaUnload(
   DDS_MEDIA *media)
{
   FILE *console;
   int retry;
   char reply[8];

#if !defined(_WIN32) && !defined(MINGW)
   ddsMTUNLOAD(gfileno(media->fp));  /* unload media */
#endif
   ddsOperator("dismount %s\n", media->title.ptr);

   return DDS_OK;
}
