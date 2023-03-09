static char rcsid[]="$Id: mtstatus.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, warning message on magnetic tape status (system devlopment)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_mtio.h"

void ddsMTStatus(
   int fd,
   const char *msg)
{
   int rc;
#if !defined(_WIN32) && !defined(MINGW) && !defined(Darwin)
   struct mtget mtiocget;
    
   errno = 0;
   rc = ioctl(fd, MTIOCGET, (char *)&mtiocget);
   
   #ifdef HOST_Sun
      ddsWarn("ddsMTStatus: %s\n"
"\trc= %i, errno= %i, %s\n"
"\tmt_type= 0x%lx, mt_dsreg= 0x%lx, mt_erreg= 0x%lx\n"
"\tmt_resid= %li, mt_fileno= %li, mt_blkno= %li\n"
"\tmt_flags= 0x%lx, mt_bf= %li\n",
            msg,
            rc, errno, strerror(errno),
            (long)mtiocget.mt_type, (long)mtiocget.mt_dsreg,
               (long)mtiocget.mt_erreg,
            (long)mtiocget.mt_resid, (long)mtiocget.mt_fileno,
               (long)mtiocget.mt_blkno,
            (long)mtiocget.mt_flags, (long)mtiocget.mt_bf);
   #elif defined(HOST_SGI)
      ddsWarn("ddsMTStatus: %s\n"
"\trc= %i, errno= %i, %s\n"
"\tmt_type= 0x%lx, mt_dposn= 0x%lx\n"
"\tmt_dsreg= 0x%lx, mt_erreg= 0x%lx\n"
"\tmt_resid= %li, mt_fileno= %li, mt_blkno= %li\n",
            msg,
            rc, errno, strerror(errno),
            (long)mtiocget.mt_type, (long)mtiocget.mt_dposn,
            (long)mtiocget.mt_dsreg, (long)mtiocget.mt_erreg,
            (long)mtiocget.mt_resid, (long)mtiocget.mt_fileno,
               (long)mtiocget.mt_blkno);
   #else
      ddsWarn("ddsMTStatus: %s\n\tstatus report deferred\n", msg);
   #endif
#endif
   return;
}
