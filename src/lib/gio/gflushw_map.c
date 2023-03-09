static char rcsid[]="$Id: gflushw_map.c 13 2009-01-06 16:56:43Z user $";
/*
FLUSH WRITE (memory mapped)

*/

#include "gio_sys.h"

int GIO_gflushw_map(  /* return 0 (error == EOF) */
   GFILE *fp)           /* GIO stream */
{
   #ifdef GIO_MMAP_IS_SUPPORTED
      HOST_LLONG_T n_flush;
      int rc = 0;

      if(fp->count_s) {
         /* secondary write cache non-empty */
         memcpy(fp->buf_w, fp->buf_s, fp->count_s);
         fp->buf_s = fp->buf_w = NULL;
         fp->count_s = 0;
      }

      n_flush = fp->buf - fp->start;
      fp->offset += n_flush;
      GIO_tune_write(fp, n_flush);
      fp->waste = 0;

      /* unmap old cache */
      if(fp->base_m && munmap(fp->base_m, fp->size_m)) {
         /* ouch... error while trying to unmap memory */
         fp->error = errno ? errno : EIO;
         rc = EOF;
      }
      fp->size_m = 0;
      fp->start = fp->buf = fp->base_m = NULL;

      if(fp->state & GIO_STATE_SEEK) {
         /* correct kernel offset for write cache */
         if(GIO_SEEK(fp, fp->offset, SEEK_SET) < 0) {
            /* seek error */
            rc = EOF;
            fp->error = errno ? errno : EIO;
         }
      }

      fp->count_w = 0;

      return rc;
   #else /* GIO_MMAP_IS_SUPPORTED */
      return EOF;
   #endif /* GIO_MMAP_IS_SUPPORTED */
}
