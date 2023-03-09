static char rcsid[]="$Id: gsetvbuf_mode.c 13 2009-01-06 16:56:43Z user $";
/*
GIO_setvbuf_mode

*/

#include "gio_sys.h"

#define MASK_IO_READ \
   (GIO_IO_COPY_READ | GIO_IO_MMAP_READ | GIO_IO_RAW_READ)

#define MASK_IO_WRITE \
   (GIO_IO_COPY_WRITE | GIO_IO_MMAP_WRITE | GIO_IO_RAW_WRITE)

int GIO_setvbuf_mode(
   GFILE *fp,           /* i/o stream */
   int *mode_buffer,    /* mode, restricted to standard. */
   int *mode_read,      /* mode, restricted to valid read  flags. */
   int *mode_write,     /* mode, restricted to valid write flags. */
   int mode)            /* mode provided by API */
{
   #ifdef GIO_RAWIO_IS_SUPPORTED
      GIO_BOOLEAN ignore_rawio;
      #ifdef HOST_SGI
         struct dioattr sgi_dio;
         int rc_fcntl;
         int rc_fcntl_F_GETFL, rc_fcntl_F_DIOINFO;
      #endif
   #endif /* GIO_RAWIO_IS_SUPPORTED */

   *mode_buffer = mode & (GIO_MASK_STANDARD_MODES | GIO_NOMINAL_BUFFERING);

   if(*mode_buffer != GIO_NOMINAL_BUFFERING &&
      *mode_buffer != _IOFBF &&
      *mode_buffer != _IOLBF &&
      *mode_buffer != _IONBF) {
      errno = EINVAL;
      return EOF;
   }

   if(*mode_buffer == GIO_NOMINAL_BUFFERING) {
      /* nominal buffering, based upon media genus */
      switch(fp->genus) {
      case GIO_MEDIA_GENUS_TAPE:
         *mode_buffer = _IONBF; /* non-buffered */
         break;
      case GIO_MEDIA_GENUS_TTY:
         *mode_buffer = _IOLBF; /* line buffered */
         break;
      case GIO_MEDIA_GENUS_FIFO:
      case GIO_MEDIA_GENUS_SOCKET:
         *mode_buffer = _IOFBF; /* full buffering */
         break;
      case GIO_MEDIA_GENUS_UNKNOWN:
      case GIO_MEDIA_GENUS_DISK:
      case GIO_MEDIA_GENUS_BLOCK:
      case GIO_MEDIA_GENUS_DIR:
      default:
         *mode_buffer = _IOFBF; /* full buffering */
         break;
      }
   }

   /* read and write kernel semantics (copy, mmap, rawio) */

   *mode_read = mode & (MASK_IO_READ & GIO_MASK_SUPPORTED_MODES);
   if(! *mode_read) *mode_read = MASK_IO_READ & GIO_DEFAULT_SPEED_MODE;

   *mode_write = mode & (MASK_IO_WRITE & GIO_MASK_SUPPORTED_MODES);
   if(! *mode_write) *mode_write = MASK_IO_WRITE & GIO_DEFAULT_SPEED_MODE;

   #ifdef GIO_RAWIO_IS_SUPPORTED
      ignore_rawio = GIO_FALSE;
      #ifdef HOST_SGI
         /* sgi supports raw (direct) i/o, constrained as follows */
         rc_fcntl_F_GETFL = XFCNTL(fp->fd, F_GETFL, 0);
         if((_IONBF != (mode & GIO_MASK_STANDARD_MODES)) &&
            (*mode_read  == GIO_IO_RAW_READ ||
             *mode_write == GIO_IO_RAW_WRITE)) {
            /* *NOT* "non-buffered" and either raw read or raw write */
            if(rc_fcntl_F_GETFL == -1) {
               /* error, ignore raw (direct) i/o advisory mode */
               ignore_rawio = GIO_TRUE;
            } else {
               /* sgi requires raw (direct) i/o for BOTH read and write */
               *mode_read = GIO_IO_RAW_READ;
               *mode_write = GIO_IO_RAW_WRITE;
               if(! (rc_fcntl_F_GETFL & FDIRECT)) {
                  /* SET the flag for direct i/o on sgi */
                  rc_fcntl_F_GETFL |= FDIRECT;
                  rc_fcntl = XFCNTL(fp->fd, F_SETFL, rc_fcntl_F_GETFL);
                  if(rc_fcntl == -1) ignore_rawio = GIO_TRUE; /* error */
               }
            }
            rc_fcntl_F_DIOINFO = XFCNTL(fp->fd, F_DIOINFO, &sgi_dio);
            if(rc_fcntl_F_DIOINFO) {
               /* error, ignore raw (direct) i/o advisory mode */
               ignore_rawio = GIO_TRUE;
            } else {
               /* save constraints for buffer alignment and i/o size */
               fp->blkalign = sgi_dio.d_mem;
               fp->minsize = sgi_dio.d_miniosz;
               fp->maxsize = sgi_dio.d_maxiosz;
            }
         } else {
            /* sgi requires raw (direct) i/o for NEITHER read nor write */
            ignore_rawio = GIO_TRUE;
         }
         if(ignore_rawio) {
            /* raw (direct) i/o is not support on this file */
            if(rc_fcntl_F_GETFL != -1 && (rc_fcntl_F_GETFL & FDIRECT)) {
               /* RESET the flag for direct i/o on sgi */
               rc_fcntl_F_GETFL &= ~ FDIRECT;
               rc_fcntl = XFCNTL(fp->fd, F_SETFL, rc_fcntl_F_GETFL);
            }
         }
      #endif /* HOST_SGI */

      if(ignore_rawio) {
         /* raw (direct) i/o is not support on this file */
         if(*mode_read == GIO_IO_RAW_READ)
            *mode_read = (GIO_IO_RAW_READ !=
               (MASK_IO_READ & GIO_DEFAULT_SPEED_MODE)) ?
               (MASK_IO_READ & GIO_DEFAULT_SPEED_MODE) : GIO_IO_COPY_READ;
         if(*mode_write == GIO_IO_RAW_WRITE)
            *mode_write = (GIO_IO_RAW_WRITE !=
               (MASK_IO_WRITE & GIO_DEFAULT_SPEED_MODE)) ?
               (MASK_IO_WRITE & GIO_DEFAULT_SPEED_MODE) : GIO_IO_COPY_WRITE;
      }
   #endif /* GIO_RAWIO_IS_SUPPORTED */

   return 0;
}
