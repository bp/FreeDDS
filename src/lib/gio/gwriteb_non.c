static char rcsid[]="$Id: gwriteb_non.c 94 2009-06-23 22:53:09Z user $";
/*
WRITE Biased pointer (non-buffered)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_gwriteb_non(  /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size,      /* requested byte count */
   GIO_ALIGN align,        /* buffer alignment */
   size_t bias)         /* alignment bias */
{
   GIO_BIG n_write, n_skip, n_need;
   GIO_BIG new_size;
   char *new_base;
   double time;
   struct tms tbuf;
   int saved_errno;

   if(! (fp->state & GIO_STATE_WRITE)) {
      if(! (fp->state & GIO_STATE_OPENED)) {
         /* open standard stream (gstdin, gstdout, gstderr) */
         if(! GIO_gopen_init(fp)) {*buf = NULL; return 0;}
      }
      if(! (fp->state & GIO_STATE_OPENW)) {
         /* write not allowed on this stream */
         errno = fp->error = EACCES; *buf = NULL; return 0;
      } else if(fp->state & GIO_STATE_READ) {
         /* flush read cache before changing to write state */
         if(fp->gflushr(fp)) {*buf = NULL; return 0;}
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
         fprintf(gio.trc_file, "WRITE: b_non fd=%d nybtes=%d time(sec)=%g\n", 
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
         *buf = NULL;
         return 0;
      }
   }

   if(! size) {*buf = NULL; return 0;}

   /* determine minimum size for primary cache */
   new_size = GIO_MALLOC_MIN *
      (1 + (size + align - 1) / GIO_MALLOC_MIN);

   if(fp->size_p < new_size) {
      /* increase size of primary cache */
      new_size = fp->mod_p * ((new_size + fp->mod_p - 1) / fp->mod_p);
      saved_errno=errno;
      new_base = (char*)GIO_MALLOC_COPY(fp, new_size);
      if(! new_base) {
	errno = fp->error = ENOMEM; 
	*buf = NULL; 
	return 0;
      } else {
	errno = fp->error = saved_errno;
      }
      if(fp->base_p) free(fp->base_p);
      fp->size_p = new_size;
      fp->base_p = new_base;
   }

   GIO_PTR_SKIP(n_skip, fp->base_p, align, bias);
   *buf = fp->start = fp->base_p + n_skip;
   fp->buf = fp->start + size;
   return size;
}
