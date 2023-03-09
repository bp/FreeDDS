static char rcsid[]="$Id: openblock.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, set block size for binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsOpenBlock(
   DDS_BIN *bin)
{
   DDS_MEDIA *media = bin->media;
   int rc = 0;

   if(bin->state & DDS_BIN_UNBLOCK) {
      /* unbuffered */
      rc = gsetvbuf(media->fp, NULL, _IONBF, 0);
      ddsVerbose("UNBLOCK rc=%d\n", rc);
   } else if(bin->block_size > 0) {
      /* full buffering */
      rc = gsetvbuf(media->fp, NULL, _IOFBF, bin->block_size);
      ddsVerbose("BLOCK size=%d rc=%d\n",(int)(bin->block_size), rc);
   }

   if(rc) {
      ddsError("%s\n\t%s\n", strerror(errno), media->title.ptr);
      return DDS_BAD;
   }

   return DDS_OK;
}
