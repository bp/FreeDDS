static char rcsid[]="$Id: gwriteb_buf.c 94 2009-06-23 22:53:09Z user $";
/*
WRITE Biased pointer (buffered)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_gwriteb_buf(  /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   GIO_BIG size,      /* requested byte count */
   GIO_ALIGN align,        /* buffer alignment */
   size_t bias)         /* alignment bias */
{
   GIO_BIG n_write, n_part, n_need, n_want, n_omit, n_flush, n_skip;
   GIO_BIG new_size;
   char *new_base, *start;
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
      fp->delta = fp->count_r = fp->count_w = 0;
      fp->state = (GIO_STATE)(GIO_STATE_WRITE |
	 (fp->state & ~ GIO_STATE_READ));
   }

   if(fp->count_s) {
      /* secondary write cache non-empty */
      memcpy(fp->buf_w, fp->buf_s, fp->count_s);
      fp->buf_s = fp->buf_w = NULL;
      fp->count_s = 0;
   }

   if(! size) {*buf = NULL; return 0;}

   /* n_need = bytes waiting to be flushed */
   n_need = fp->buf - fp->start;
   if(n_need) {
#ifdef SNAFU_BLOCKED_TAPE_READ
   /* 9/28/97 rls, screws up blocked tape read */
      n_omit = fp->start - fp->base_p;
#else
   n_omit = 0;
#endif
   } else {
#ifdef SNAFU_BLOCKED_TAPE_READ
      /* 9/28/97 rls, screws up blocked tape read */
      n_omit = HOST_INT6_MOD(fp->offset, fp->mod_w);
#else
      n_omit = 0;
#endif
      fp->start = fp->buf = fp->base_p + n_omit;
      fp->waste = fp->size_p - n_omit;
      fp->count_w = fp->waste;
   }

   if(fp->count_w >= size) {
      /* required space already cached (primary) */
      if(GIO_PTR_BIASED(fp->buf, align, bias)) {
         /* cache address satisfies biased alignment */
         *buf = fp->buf;
      } else {
         /* secondary cache required to align data */
         new_size = GIO_MALLOC_MIN *
            (1 + (size + align - 1) / GIO_MALLOC_MIN);
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
            fp->size_s = new_size;
            fp->base_s = new_base;
         }
         /* save pointers for secondary write cache */
         GIO_PTR_SKIP(n_skip, fp->base_s, align, bias);
         *buf = fp->buf_s = fp->base_s + n_skip;
         fp->buf_w = fp->buf;
         fp->count_s = size;
      }
      fp->buf += size;
      fp->count_w -= size;
      return size;
   }

   n_part = (n_omit + n_need) % fp->mod_w;
   n_want = n_need - n_part;
   if(n_want && n_want >= fp->min_w) {
      /* flush bytes not wanted from write cache */
      start = fp->start;
      while(n_want > 0) {
         if(gio.trc_file) {
            time = times(&tbuf);
            n_write = XWRITE(fp->fd, start, n_want);
            time = (times(&tbuf) - time)/gio.clk_tck;
            fprintf(gio.trc_file, "WRITE: b_buf fd=%d nybtes=%d time(sec)=%g\n", 
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
         fp->waste = -1; /* more is *always* better */
         fp->offset += n_flush;
         GIO_tune_write(fp, n_flush);
         fp->start = (char*)start;
         fp->waste = fp->size_p - (start - fp->base_p);
      }
      if(n_want) {
         /* ouch... some data was NOT flushed ?-( */
         fp->error = errno ? errno : EIO;
         *buf = NULL;
         return 0;
      }
      n_need -= n_flush;
      n_omit = 0;
   }

   /* minimum size for primary cache */
   new_size = n_omit + n_need + size;
   if(new_size < fp->cache_w) new_size = fp->cache_w;

   /* maximum retained data is 10% (nominal is 5%) */
   if(new_size < 10 * fp->mod_w) new_size = 10 * fp->mod_w;

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
      if(n_need) {
         /* retain data already within primary cache */
         memcpy(new_base + n_omit, fp->start, n_need);
      }
      if(fp->base_p) free(fp->base_p);
      fp->size_p = new_size;
      fp->base_p = new_base;
   } else if(n_need && fp->start != fp->base_p + n_omit) {
      /* retain data already within primary cache */
      memmove(fp->base_p + n_omit, fp->start, n_need);
   }

   fp->start = fp->base_p + n_omit;
   fp->buf = fp->start + n_need;
   fp->waste = fp->size_p - n_omit;
   fp->count_w = fp->waste - n_need;

   if(GIO_PTR_BIASED(fp->buf, align, bias)) {
      /* cache address satisfies biased alignment */
      *buf = fp->buf;
   } else {
      /* secondary cache required to align data */
      new_size = GIO_MALLOC_MIN *
         (1 + (size + align - 1) / GIO_MALLOC_MIN);
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
      /* save pointers for secondary write cache */
      GIO_PTR_SKIP(n_skip, fp->base_s, align, bias);
      *buf = fp->buf_s = fp->base_s + n_skip;
      fp->buf_w = fp->buf;
      fp->count_s = size;
   }
   fp->buf += size;
   fp->count_w -= size;
   return size;
}
