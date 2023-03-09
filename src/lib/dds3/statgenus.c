static char rcsid[]="$Id: statgenus.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, media genus via stat (filename)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_MEDIA_GENUS ddsStatGenus(
   const char *name)
{
   DDS_MEDIA_GENUS genus;
   STRUCTSTAT stat_buf;

   /* Sun debugger rui (read from uninitialized) snafu */
   memset((void*)&stat_buf, '\0', sizeof(stat_buf));

   if(! ddsSTAT(name, &stat_buf)) {
      if(S_ISCHR(stat_buf.st_mode)) {
         genus = DDS_MEDIA_CHR;
      } else if(S_ISBLK(stat_buf.st_mode)) {
         genus = DDS_MEDIA_BLK;
      } else if(S_ISREG(stat_buf.st_mode)) {
         genus = DDS_MEDIA_REG;
      } else if(S_ISDIR(stat_buf.st_mode)) {
         genus = DDS_MEDIA_DIR;
      } else if(S_ISFIFO(stat_buf.st_mode)) {
         genus = DDS_MEDIA_FIFO;
      } else {
         genus = DDS_MEDIA_NOT;
      }
   } else {
      genus = DDS_MEDIA_ERR;
   }

   if(! strncmp(name, "/dev/tty", 8)) {
      genus = DDS_MEDIA_TTY;
   }

   if(! strncmp(name, "/dev/null", 9)) {
      genus = DDS_MEDIA_NULL;
   }

   return genus;
}
