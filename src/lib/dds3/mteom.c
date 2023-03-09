static char rcsid[]="$Id: mteom.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, true if (tape) drive status is eom

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_mtio.h"

DDS_BOOLEAN ddsMTEOM(
   int fd)
{
   int rc;

#if !defined(_WIN32) && !defined(MINGW) && !defined(Darwin)
   /* see Sun man page mtio.4 */
   struct mtget mt_status;

   errno = 0;
   rc = ioctl(fd, MTIOCGET, (char *)&mt_status);
   if(! rc && 19 == mt_status.mt_erreg) return DDS_TRUE;
#endif
   return DDS_FALSE;
}
