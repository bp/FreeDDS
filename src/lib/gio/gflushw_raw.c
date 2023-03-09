static char rcsid[]="$Id: gflushw_raw.c 13 2009-01-06 16:56:43Z user $";
/*
FLUSH WRITE (raw/direct)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_gflushw_raw(  /* return 0 (error == EOF) */
   GFILE *fp)           /* GIO stream */
{
   #ifdef GIO_RAWIO_IS_SUPPORTED
      HOST_LLONG_T n_write, n_want, n_flush;
      char *start;
      int rc = 0;
      double time;
      struct tms tbuf;

      if(fp->count_s) {
         /* secondary write cache non-empty */
         memcpy(fp->buf_w, fp->buf_s, fp->count_s);
         fp->buf_s = fp->buf_w = NULL;
         fp->count_s = 0;
      }

      n_want = fp->buf - fp->start;
      if(n_want) {
         /* write data from cache */
         start = fp->start;
         while(n_want > 0) {
            if(gio.trc_file) {
               time = times(&tbuf);
               n_write = XWRITE(fp->fd, start, n_want);
               time = (times(&tbuf) - time)/gio.clk_tck;
               fprintf(gio.trc_file, "WRITE: flushw_raw fd=%d nybtes=%d time(sec)=%g\n", 
                       fp->fd, (int)n_write, time);
            } else {
               n_write = XWRITE(fp->fd, start, n_want);
            }
            if(n_write <= 0) break; /* write problem */
            n_want -= n_write;
            start += n_write;
         }

         n_flush = start - fp->start;
         if(n_flush) {
            /* some data was flushed */
            fp->start = start;
            fp->offset += n_flush;
            GIO_tune_write(fp, n_flush);
         }
         if(n_want) {
            /* ouch... some data was NOT flushed ?-( */
            fp->error = errno ? errno : EIO;
            rc = EOF;
         }
      }
      fp->count_w = 0;
      fp->waste = 0;

      return rc;

   #else    /* GIO_RAWIO_IS_SUPPORTED */
      return EOF;
   #endif    /* GIO_RAWIO_IS_SUPPORTED */
}
