static char rcsid[]="$Id: gsetvbuf_stat.c 13 2009-01-06 16:56:43Z user $";
/*
GIO_setvbuf_stat

*/

#include "gio_sys.h"
#if !defined(_WIN32) && !defined(MINGW) && !defined(Darwin)
#include <sys/vfs.h>
#endif

/* determine if we are running on panfs */
#define PAN_KERNEL_FS_CLIENT_SUPER_MAGIC 0xAAD7AAEA

int GIO_setvbuf_stat(   /* status (non-zero == error) */
   GFILE *fp)           /* i/o stream */
{
   struct GIO_STAT stat_buf;
   GIO_MEDIA_GENUS genus = GIO_MEDIA_GENUS_UNKNOWN;
   int stat_rc;
#ifdef HOST_Linux
   struct statfs  statfs_buf;
#endif

#if defined(_WIN32) || defined(MINGW)
   stat_rc = XFSTAT(fp->cfd.name, &stat_buf);
#else
   stat_rc = XFSTAT(fp->fd, &stat_buf);
#endif
   if(! stat_rc) {
      /* extract information from stat_buf */
#if defined(_WIN32) || defined(MINGW)
      if (S_IFREG == (stat_buf.st_mode & S_IFMT)) {
         genus = GIO_MEDIA_GENUS_DISK;
      } else if (S_IFDIR == (stat_buf.st_mode & S_IFMT)) {
         genus = GIO_MEDIA_GENUS_DIR;
      } else {
         genus = GIO_MEDIA_GENUS_UNKNOWN;
      }
#else
      if(S_IFREG == (stat_buf.st_mode & S_IFMT)) {
         genus = GIO_MEDIA_GENUS_DISK;
#ifdef HOST_sun4cg92
      } else if(isastream(fp->fd) == 1) {
         genus = GIO_MEDIA_GENUS_SOCKET;
#else
      } else if(S_IFIFO == (stat_buf.st_mode & S_IFMT)) {
         genus = GIO_MEDIA_GENUS_FIFO;
      } else if(S_IFSOCK == (stat_buf.st_mode & S_IFMT)) {
         genus = GIO_MEDIA_GENUS_SOCKET;
#endif
      } else if(S_IFCHR == (stat_buf.st_mode & S_IFMT) ||
                S_IFBLK == (stat_buf.st_mode & S_IFMT)) {
         genus = GIO_MEDIA_GENUS_TAPE;
      } else if(S_IFDIR == (stat_buf.st_mode & S_IFMT)) {
         genus = GIO_MEDIA_GENUS_DIR;
      } else {
         genus = GIO_MEDIA_GENUS_UNKNOWN;
      }
#endif
      if(0 < stat_buf.st_size) {
         fp->eof_size = stat_buf.st_size;
      }
#if defined(_WIN32) || defined(MINGW)
      fp->blksize = 512;
#else
      if(0 < stat_buf.st_blksize) {
         fp->blksize = stat_buf.st_blksize;
         if (genus == GIO_MEDIA_GENUS_DISK) {
#ifdef HOST_IRIX64
         /* We cannot let this grow to the xfs st_blksize of 42M          */
         /* because when we open 1000's files it ties up too much memory. */
         /* So we will just set it at 512k; no real speedup going larger. */
         /* J. Ehlers Oct. 2003                                           */
         /* if (fp->blksize < 524288 && genus == GIO_MEDIA_GENUS_DISK) {  */
            fp->blksize = 524288;
#endif
#ifdef HOST_Linux
         /* in the Panasas case, set the blocksize to 39*64K = 2555904 */
         /* Jan 2007 - set back to 256k without much loss because of problems
            with Altix kernel with "unable to allocate memory" on write errors
            randomly cropping up */
            if (fstatfs(fp->fd, &statfs_buf) >= 0
                && statfs_buf.f_type == PAN_KERNEL_FS_CLIENT_SUPER_MAGIC) {
               /* fp->blksize = 2555904; */
               fp->blksize = 262144;
            }
#endif
            /* Allow environment, GIO_BLKSIZE, to set the cache sizes */
            if (gio.blk_size > 0) {
               fp->blksize = gio.blk_size;
            }
         }
      }
#endif
   }

   if(isatty(fp->fd)) genus = GIO_MEDIA_GENUS_TTY;

   fp->genus = genus;

   return stat_rc;
}
