static char rcsid[]="$Id: gflushw_non.c 13 2009-01-06 16:56:43Z user $";
/*
FLUSH WRITE (non-buffered)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_gflushw_non(  /* return 0 (error == EOF) */
   GFILE *fp)           /* GIO stream */
{
   HOST_LLONG_T n_write, n_want;
   int rc = 0;
   double time;
   struct tms tbuf;

   n_want = fp->buf - fp->start;
   if(n_want) {
      if(gio.trc_file) {
         time = times(&tbuf);
         n_write = XWRITE(fp->fd, fp->start, n_want);
         time = (times(&tbuf) - time)/gio.clk_tck;
         fprintf(gio.trc_file, "WRITE: flushw_non fd=%d nybtes=%d time(sec)=%g\n", 
                 fp->fd, (int)n_write, time);
      } else {
         n_write = XWRITE(fp->fd, fp->start, n_want);
      }

      if(n_write > 0) {
         n_want -= n_write;
         fp->start += n_write;
         fp->offset += n_write;
      }
      if(n_want) {
         /* ouch... *some* data was NOT written ?-( */
         fp->error = errno ? errno : EIO;
         rc = EOF;
      }
   }
   fp->waste = 0;

   return rc;
}
