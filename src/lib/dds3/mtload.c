static char rcsid[]="$Id: mtload.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, load (online) on (tape) drive

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_mtio.h"

int ddsMTLOAD(
   int fd)
{
#if defined(HOST_sun4)
   ddsPanic("ddsMTLOAD: deferred\n");
#elif defined(HOST_Sun)
   #ifdef BAD_IDEA
      /* 28 Aug 1998, RLS... MTLOAD is no longer documented in mtio.4 */
      /* It seems to work on Solaris using 8mm, but not DLT. */
         /* see Sun man page mtio.4 */
         struct mtop mt_command;

         errno = 0;
         mt_command.mt_op = MTLOAD;
         mt_command.mt_count = 1;
         if(ioctl(fd, MTIOCTOP, &mt_command)) return DDS_BAD;
   #endif

   struct mtop mt_command;

   errno = 0;
   mt_command.mt_op = MTREW;
   mt_command.mt_count = 1;

   /* attempt an operation (error if device is not ready yet) */
   if(ioctl(fd, MTIOCTOP, &mt_command)) {
      return DDS_BAD;
   }
#elif defined(HOST_SGI)
   struct mtop mt_command;

   errno = 0;
   mt_command.mt_op = MTREW;
   /* attempt an operation (error if device is not ready yet) */
   if(ioctl(fd, MTIOCTOP, &mt_command)) {
      return DDS_BAD;
   }
#else
   ddsPanic("ddsMTLOAD: deferred\n");
#endif

   return DDS_OK;
}
