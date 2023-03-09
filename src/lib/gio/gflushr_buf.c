static char rcsid[]="$Id: gflushr_buf.c 13 2009-01-06 16:56:43Z user $";
/*
FLUSH READ (buffered)

*/

#include "gio_sys.h"

int GIO_gflushr_buf(   /* return 0 (error == EOF) */
   GFILE *fp)           /* GIO stream */
{
   /* ANSI standard implies that flush on read is undefined. */
   /* This is my unofficial definition of flush on read stream */
   HOST_LLONG_T n_flush;
   int rc = 0;

   if(fp->state & GIO_STATE_UNREAD) {
      /* flush the unread cache */
      fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
      fp->buf = fp->buf_u;
      fp->count_r = fp->count_u;
   }

   n_flush = fp->buf - fp->start;
   fp->offset += n_flush;
   GIO_tune_read(fp, n_flush);
   fp->waste = 0;

   if(fp->base_p) {
#ifdef SNAFU_BLOCKED_TAPE_READ
      /* 9/28/97 rls, screws up blocked tape read */
      fp->start = fp->buf =
         fp->base_p + HOST_INT6_MOD(fp->offset, fp->mod_r);
#else
      fp->start = fp->buf = fp->base_p;
#endif
   }

   if(fp->count_r && fp->state & GIO_STATE_SEEK) {
      /* correct kernel offset for read ahead cache */
      if(GIO_SEEK(fp, fp->offset, SEEK_SET) < 0) {
         /* seek error */
         rc = EOF;
         fp->error = errno ? errno : EIO;
      }
   }

   fp->state &= (GIO_STATE) ~ GIO_STATE_KEOF;
   fp->count_r = 0; 

   return rc;
}
