static char rcsid[]="$Id: fstatid.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, initialize file stat id, given its file descriptor.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsFStatID(
   DDS_STAT_ID *stat_id,
   int fd)
{
   STRUCTSTAT stat_buf;

   /* Sun debugger rui (read from uninitialized) snafu */
   memset((void*)&stat_buf, '\0', sizeof(stat_buf));

   if(ddsFSTAT(fd, &stat_buf)) {
      memset((void*)stat_id, '\0', sizeof(DDS_STAT_ID));
      return DDS_BAD;
   }

   memcpy((void*)&stat_id->st_dev, (void*)&stat_buf.st_dev, sizeof(DDS_STAT_ID));
   stat_id->st_ino = stat_buf.st_ino;
   return DDS_OK;
}
