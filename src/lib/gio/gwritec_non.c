static char rcsid[]="$Id: gwritec_non.c 13 2009-01-06 16:56:43Z user $";
/*
WRITE copy buffer (non-buffered)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_gwritec_non(  /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* write buffer pointer */
   GIO_BIG size)      /* requested byte count */
{
   double time;
   struct tms tbuf;
   GIO_BIG n_write, n_need;

   if(! (fp->state & GIO_STATE_WRITE)) {
      if(! (fp->state & GIO_STATE_OPENED)) {
         /* open standard stream (gstdin, gstdout, gstderr) */
         if(! GIO_gopen_init(fp)) {return 0;}
      }
      if(! (fp->state & GIO_STATE_OPENW)) {
         /* write not allowed on this stream */
         errno = fp->error = EACCES; return 0;
      } else if(fp->state & GIO_STATE_READ) {
         /* flush read cache before changing to write state */
         if(fp->gflushr(fp)) {return 0;}
      }
      /* change to write state */
      fp->start = fp->buf = fp->base_p;
      fp->state = (GIO_STATE)(GIO_STATE_WRITE |
	 (fp->state & ~ GIO_STATE_READ));
   }

   n_need = fp->buf - fp->start;
   if(n_need) {
      /* need to flush data from write cache */
      if(gio.trc_file) {
         time = times(&tbuf);
         n_write = XWRITE(fp->fd, fp->start, n_need);
         time = (times(&tbuf) - time)/gio.clk_tck;
         fprintf(gio.trc_file, "WRITE: c_non fd=%d nybtes=%d time(sec)=%g\n", 
                 fp->fd, (int)n_write, time);
      } else {
         n_write = XWRITE(fp->fd, fp->start, n_need);
      }
      if(n_write > 0) {
         /* some data was flushed */
         n_need -= n_write;
         fp->offset += n_write;
         fp->start = fp->buf = fp->base_p;
      }
      if(n_need) {
         /* ouch... *some* data was NOT written ?-( */
         fp->error = errno ? errno : EIO;
         return 0;
      }
   }

   if(! size) return 0;

   if(gio.trc_file) {
      time = times(&tbuf);
      n_write = XWRITE(fp->fd, buf, size);
      time = (times(&tbuf) - time)/gio.clk_tck;
      fprintf(gio.trc_file, "WRITE: c_non fd=%d nybtes=%d time(sec)=%g\n", 
              fp->fd, (int)n_write, time);
   } else {
      n_write = XWRITE(fp->fd, buf, size);
   }
   if(n_write > 0) {
      /* some data was flushed */
      fp->offset += n_write;
      if(n_write < size) {
         /* ouch... *some* data was NOT written ?-( */
         fp->error = errno ? errno : EIO;
      }
      return n_write;
   }

   /* no data was written */
   fp->error = errno ? errno : EIO;
   return 0;
}
