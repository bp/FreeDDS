static char rcsid[]="$Id: fstatsize.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, media genus via fstat (file descriptor)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

off_t ddsFstatSize(
   int fd)
{
   STRUCTSTAT stat_buf;

   if(! ddsFSTAT(fd, &stat_buf)) {
      return stat_buf.st_size;
   }

   return -1;
}
