static char rcsid[]="$Id: gsetpos.c 13 2009-01-06 16:56:43Z user $";
/*
SET POSition (offset)

*/

#include "gio_sys.h"
 
int GIO_setpos(         /* status (ok == 0, error < 0) */
   GFILE *fp,           /* i/o stream */
   const gpos_t *offset) /* huge seek offset */
{
   HOST_LLONG_T n_omit, n_part, n_keep, n_flush;
 
   /* initialize gstdin, gstdout, or gstderr, prior to access */
   if(! (fp->state & GIO_STATE_OPENED) &&
      ! GIO_gopen_init(fp)) return EOF;
 
   if(fp->state & GIO_STATE_WRITE) {
      /* stream is writing */
      n_flush = fp->buf - fp->start;
      if(fp->offset + n_flush == *offset) {
         /* ignore seek (new position equals current position) */
         return 0;
      }
      /* flush write buffer before seek */
      if(fp->gflushw(fp)) return EOF;

   } else if(fp->state & GIO_STATE_READ) {
      /* stream is reading */
      if(fp->state & GIO_STATE_UNREAD) {
         /* discard unread buffer, and change to primary buffer */
         fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
         if(fp->state & GIO_MASK_BUFFERED) {
            fp->buf = fp->buf_u;
            fp->count_r = fp->count_u;
         }
      }

      n_flush = fp->buf - fp->start;
      n_keep = n_flush + fp->count_r;
      if(fp->offset + n_keep >= *offset) {
         /* new offset is less than or equal to primary cache end */
         n_part = *offset - fp->offset;
         if(n_part >= 0) {
            /* new offset is greater than or equal to primary cache start */
            /* simply change logical position within read cache */
            fp->state &= (GIO_STATE) ~ GIO_STATE_EOF;
            fp->waste += n_part - n_flush;
            fp->count_r = n_keep - n_part;
            fp->buf = fp->start + n_part;
            return 0;
         }
      }

      /* seek is outside of read cache */
      /* flush read buffer before seek */
      if(fp->gflushr(fp)) return EOF;
   }

   #ifdef GIO_RAWIO_IS_SUPPORTED
      if(fp->state & (GIO_STATE_RAWR | GIO_STATE_RAWW)) {
         /* avoid sgi seek errno = 22 */
         n_omit = HOST_INT6_MOD(*offset, fp->mod_r);
      } else {
         n_omit = 0;
      }
   #else
      n_omit = 0;
   #endif

   if(! n_omit) {
      if(GIO_SEEK(fp, *offset, SEEK_SET) < 0) {
         fp->error = errno ? errno : EIO;
         return EOF;
      }
   }
   fp->offset = *offset;

   fp->state &= (GIO_STATE) ~ GIO_STATE_EOF;
   return 0;
}

int gsetpos(            /* status (error < 0) */
   GFILE *fp,           /* i/o stream */
   const gpos_t *offset) /* huge seek offset */
{
   if(GIO_cfd_check(fp) < 0) return -1;

   return GIO_setpos(fp, offset);
}
