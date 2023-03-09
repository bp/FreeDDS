#ifndef __GIO_SYS_H
#define __GIO_SYS_H

/*
RCS-t=GIO, include file for internal functions.

$Id: gio_sys.h 199 2010-11-04 17:14:10Z user $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <limits.h>
#if defined(_WIN32) || defined(MINGW)
#include <io.h>
#else
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include "gio.h"
#include "gio_config.h"
#include "gio_types.h"
#include "gio_proto.h"
#include "gio_macro.h"

#ifdef GIO_MMAP_IS_SUPPORTED
   #include <sys/mman.h>
#endif /* GIO_MMAP_IS_SUPPORTED */

/* one, and only one, module should #define GIO_GLOBAL */
#ifdef GIO_GLOBAL
 
   #define GIO_FUNC \
      &GIO_int_defer, \
      &GIO_int_defer, \
      &GIO_greadb_buf, \
      &GIO_greadc_buf, \
      &gunread, \
      &GIO_gflushr_buf, \
      &GIO_int_defer, \
      &GIO_int_defer, \
      &GIO_gwriteb_buf, \
      &GIO_gwritec_buf, \
      &GIO_gflushw_buf
 
   #define GIO_stdin_fd  0
   #define GIO_stdout_fd 1
   #define GIO_stderr_fd 2
 
   static GFILE GIO_stdin  = {GIO_MAGIC, 0, 0,
      NULL, NULL,
         (GIO_STATE)(GIO_STATE_OPENR | GIO_STATE_KEEPFB),
      GIO_stdin_fd, GIO_FUNC};
   static GFILE GIO_stdout = {GIO_MAGIC, 0, 0,
      NULL, NULL,
         (GIO_STATE)(GIO_STATE_OPENW | GIO_STATE_KEEPFB),
      GIO_stdout_fd, GIO_FUNC};
   static GFILE GIO_stderr = {GIO_MAGIC, 0, 0,
      NULL, NULL,
      (GIO_STATE)(GIO_STATE_OPENW | GIO_STATE_KEEPFB |
         GIO_STATE_LINEBUF),
      GIO_stderr_fd, GIO_FUNC};
 
   int gio_mpi_node = -1;

   GFILE * const gstdin = &GIO_stdin;
   GFILE * const gstdout = &GIO_stdout;
   GFILE * const gstderr = &GIO_stderr;

   GIO_ROOT gio = {(~ GIO_MAGIC), 0, NULL, NULL, NULL, NULL, 0, GIO_OPEN_MAX, 
                   0, 0, 0, 0, 0};

#endif /* GIO_GLOBAL */

#if defined(_WIN32) || defined(MINGW)   /* Only Windows supports these */
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

#endif /* __GIO_SYS_H */
