static char rcsid[]="$Id: greadc_non.c 13 2009-01-06 16:56:43Z user $";
/*
READ copy buffer (non-buffered)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_greadc_non(   /* actual byte count (error < 0) */
   GFILE *fp,         /* i/o stream */
   void *buf,         /* read buffer pointer */
   GIO_BIG size)      /* requested byte count */
{
   HOST_LLONG_T n_read;
   double time;
   struct tms tbuf;

   /* nonbuffered implications...
      count_s, count_r and count_w should always be zero.
      The unread buffer is supported, but its always consumed and
      flushed by any subsequent read with a size greater than zero.
      Buf_u and count_u are used to control the unread buffer.
   */

   if(! (fp->state & GIO_STATE_READ)) {
      if(! (fp->state & GIO_STATE_OPENED)) {
         /* open standard stream (gstdin, gstdout, gstderr) */
         if(! GIO_gopen_init(fp)) {return 0;}
      }
      if(! (fp->state & GIO_STATE_OPENR)) {
         /* read not allowed on this stream */
         errno = fp->error = EACCES; return 0;
      } else if(fp->state & GIO_STATE_WRITE) {
         /* flush write cache before changing to read state */
         if(fp->gflushw(fp)) {return 0;}
      }
      /* change to read state */
      fp->start = fp->buf = fp->base_p;
      fp->state = (GIO_STATE)(GIO_STATE_READ |
	 (fp->state & ~ GIO_STATE_WRITE));
   }

   if(! size) return 0;

   if(fp->state & GIO_STATE_UNREAD) {
      /* nonbuffered, empty the entire unread buffer */
      n_read = (size < fp->count_u) ? size : fp->count_u;
      memcpy(buf, fp->buf_u, n_read);
#ifdef AXIAN_REL_ALPHA_07
/* 7 May 1997 RLS, I think this is undesireable. */
/* Please enlighten me if you disagree. */
      /* MRL *** */
      if(n_read < fp->count_u) {
         fp->count_u -=  n_read;
         fp->buf_u +=  n_read;
      } else {
         fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
      }
      /* MRL *** */
      if( fp->genus == GIO_MEDIA_GENUS_FIFO || fp->genus == GIO_MEDIA_GENUS_SOCKET) {
         if( n_read != size) {
            if(gio.trc_file) {
               time = times(&tbuf);
               n_read += XREAD(fp->fd, (char *)buf + n_read, size - n_read);
               time = (times(&tbuf) - time)/gio.clk_tck;
               fprintf(gio.trc_file, "READ: c_non fd=%d nybtes=%d time(sec)=%g\n", 
                       fp->fd, (int)n_read, time);
            } else {
               n_read += XREAD(fp->fd, (char *)buf + n_read, size - n_read);
            }
        }
      }
#else
      fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
#endif
      return n_read;
   }

   if(gio.trc_file) {
      time = times(&tbuf);
      n_read = XREAD(fp->fd, buf, size);
      time = (times(&tbuf) - time)/gio.clk_tck;
      fprintf(gio.trc_file, "READ: c_non fd=%d nybtes=%d time(sec)=%g\n", 
              fp->fd, (int)n_read, time);
   } else {
      n_read = XREAD(fp->fd, buf, size);
   }
   if(n_read > 0) {
      fp->offset += n_read;
      fp->state &= (GIO_STATE)(~ GIO_STATE_EOF);
      return n_read;
   }

   if(! n_read) {
      fp->state |= GIO_STATE_EOF;
   } else {
      fp->error = errno ? errno : EIO;
   }
 
   return 0;
}
