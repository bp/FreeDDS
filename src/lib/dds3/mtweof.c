static char rcsid[]="$Id: mtweof.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, write eof on (tape) drive

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_mtio.h"

int ddsMTWEOF(
   int fd,
   int count)
{
   int rc;
#if !defined(_WIN32) && !defined(MINGW) && !defined(Darwin)
   /* see Sun man page mtio.4 */
   struct mtop mt_command;

   errno = 0;
   mt_command.mt_op = MTWEOF;
   mt_command.mt_count = count;
   rc = ioctl(fd, MTIOCTOP, &mt_command);
   if(rc) return DDS_BAD;
#endif
   return DDS_OK;
}
