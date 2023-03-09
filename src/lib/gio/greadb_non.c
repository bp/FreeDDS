static char rcsid[]="$Id: greadb_non.c 94 2009-06-23 22:53:09Z user $";
/*
READ Biased pointer (non-buffered)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_greadb_non(   /* actual byte count (error < 0) */
   GFILE *fp,         /* i/o stream */
   const void **buf,  /* read buffer pointer to pointer */
   GIO_BIG size,      /* requested byte count */
   GIO_ALIGN align,   /* buffer alignment */
   size_t bias)       /* alignment bias */
{
   HOST_LLONG_T n_read, n_skip;
   HOST_LLONG_T new_size;
   char *new_base;
   double time;
   struct tms tbuf;
   int saved_errno;

   /* nonbuffered implications...
      count_s, count_r and count_w should always be zero.
      The unread buffer is supported, but its always consumed and
      flushed by any subsequent read with a size greater than zero.
      Buf_u and count_u are used to remember the unread buffer.
   */

   if(! (fp->state & GIO_STATE_READ)) {
      if(! (fp->state & GIO_STATE_OPENED)) {
         /* open standard stream (gstdin, gstdout, gstderr) */
         if(! GIO_gopen_init(fp)) {*buf = NULL; return 0;}
      }
      if(! (fp->state & GIO_STATE_OPENR)) {
         /* read not allowed on this stream */
         *buf = NULL; errno = fp->error = EACCES; return 0;
      } else if(fp->state & GIO_STATE_WRITE) {
         /* flush write cache before changing to read state */
         if(fp->gflushw(fp)) {*buf = NULL; return 0;}
      }
      /* change to read state */
      fp->start = fp->buf = fp->base_p;
      fp->state = (GIO_STATE)(GIO_STATE_READ |
	 (fp->state & ~ GIO_STATE_WRITE));
   }

   if(! size) {*buf = NULL; return 0;}

   if(fp->state & GIO_STATE_UNREAD) {
      /* nonbuffered, empty the entire unread buffer */
      n_read = (size < fp->count_u) ? size : fp->count_u;
      if(GIO_PTR_BIASED(fp->buf_u, align, bias)) {
         /* cache address satisfies biased alignment */
         *buf = fp->buf_u;
      } else {
         /* secondary cache required to align data */
         new_size = GIO_MALLOC_MIN *
            (1 + (n_read + align - 1) / GIO_MALLOC_MIN);
         if(fp->size_s < new_size) {
            /* increase size of secondary cache */
	   saved_errno=errno;
	   new_base = (char*)malloc(new_size);
	   if(! new_base) {
	     errno = fp->error = ENOMEM; 
	     *buf = NULL; 
	     return 0;
	   } else {
	     errno = fp->error = saved_errno;
	   }
            if(fp->base_s) free(fp->base_s);
            fp->base_s = new_base;
            fp->size_s = new_size;
         }
         /* copy data into secondary cache */
         GIO_PTR_SKIP(n_skip, fp->base_s, align, bias);
         *buf = fp->base_s + n_skip;
         memcpy(fp->base_s + n_skip, fp->buf, n_read);
      }

#ifdef AXIAN_REL_ALPHA_07
/* 7 May 1997 RLS, I think this is undesireable. */
/* Please enlighten me if you disagree. */
      /* MRL *** */
      if(n_read < fp->count_u){
         fp->count_u -=  n_read;
         fp->buf_u +=  n_read;
      } else {
         fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
      }
#else
      fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
#endif

      return n_read;
   }

   /* minimum size for primary buffer */
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
   fp->start = fp->buf = fp->base_p + n_skip; 

   /* read the physical stream, once only */
   if(gio.trc_file) {
      time = times(&tbuf);
      n_read = XREAD(fp->fd, fp->start, size);
      time = (times(&tbuf) - time)/gio.clk_tck;
      fprintf(gio.trc_file, "READ: b_non fd=%d nybtes=%d time(sec)=%g\n", 
              fp->fd, (int)n_read, time);
   } else {
      n_read = XREAD(fp->fd, fp->start, size);
   }
   if(n_read > 0) {
      fp->offset += n_read;
      *buf = fp->start;
      fp->state &= (GIO_STATE)(~ GIO_STATE_EOF);
      return n_read;
   }

   *buf = NULL;

   if(! n_read) {
      fp->state |= GIO_STATE_EOF;
   } else {
      fp->error = errno ? errno : EIO;
   }

   return 0;
}
