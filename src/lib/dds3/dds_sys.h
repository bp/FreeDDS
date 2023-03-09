#ifndef __DDS_SYS_H
#define __DDS_SYS_H

/*
RCS-t=DDS, private "sys" (internal API)

$Id: dds_sys.h 39 2009-01-13 23:00:15Z user $
*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/
 
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#if defined(_WIN32) || defined(MINGW)
#include <io.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>

#define FUNCPROTO 1

#include "gio.h"
#include "cdds.h"
#include "dds_config.h"
#include "dds_api.h"
#include "dds_stack.h"
#include "dds_util.h"
#include "dds_string.h"
#include "dds_table.h"
#include "dds_state.h"
#include "dds_ptype.h"
#include "dds_media.h"
#include "dds_dict.h"
#include "dds_message.h"
#include "dds_symbol.h"
#include "dds_hdr.h"
#include "dds_binary.h"

/* -32 on sgi uses stat which can't always handle large file systems */
#if defined(_WIN32) || defined(MINGW)
   #define STRUCTSTAT struct __stat64
   #define ddsSTAT    _stat64
   #define ddsFSTAT   _fstat64
   #define ddsLSTAT   _lstat64
#elif IRIX32
   #define STRUCTSTAT struct stat64
   #define ddsSTAT    stat64
   #define ddsFSTAT   fstat64
   #define ddsLSTAT   lstat64
#else
   #define STRUCTSTAT struct stat
   #define ddsSTAT    stat
   #define ddsFSTAT   fstat
   #define ddsLSTAT   lstat
#endif

#if defined(_WIN32) || defined(MINGW)	/* Only Windows supports these */
#ifndef S_IFIFO
#define S_IFIFO    0x1000   /* FIFO                                            */
#endif
/* #define S_IFCHR 0x2000   /* Character device                                */
/* #define S_IFDIR 0x4000   /* Directory                                       */
#ifndef S_IFBLK
#define S_IFBLK    0x6000   /* Block device                                    */
#endif
/* #define S_IFREG 0x8000   /* Regular file                                    */
#ifndef S_IFLNK
#define S_IFLNK    0xA000   /* Symbolic link                                   */
#endif
#ifndef S_IFSOCK
#define S_IFSOCK   0xC000   /* Socket                                          */
#endif
#ifndef S_ISTYPE
#define S_ISTYPE(m, TYPE) ((m & S_IFMT) == TYPE)
#endif
/* Both DOS and Windows support these file types */
#ifndef S_ISCHR
#define S_ISCHR(m)  S_ISTYPE(m, S_IFCHR)  /* Test for a character device */
#endif
#ifndef S_ISDIR
#define S_ISDIR(m)  S_ISTYPE(m, S_IFDIR)  /* Test for a directory */
#endif
#ifndef S_ISREG
#define S_ISREG(m)  S_ISTYPE(m, S_IFREG)  /* Test for a regular file */
#endif
#ifndef S_ISLNK
#define S_ISLNK(m)  S_ISTYPE(m, S_IFLNK)  /* Test for a symbolic link */
#endif
#ifndef S_ISBLK
#define S_ISBLK(m)  S_ISTYPE(m, S_IFBLK)  /* Test for a block device */
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(m) S_ISTYPE(m, S_IFIFO)  /* Test for a pipe or FIFO */
#endif
#ifndef S_ISSOCK
#define S_ISSOCK(m) S_ISTYPE(m, S_IFSOCK) /* Test for a socket */
#endif
#endif

#endif /* __DDS_SYS_H */
