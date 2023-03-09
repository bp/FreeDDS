static char rcsid[]="$Id: gwritec_buf.c 94 2009-06-23 22:53:09Z user $";
/*
WRITE copy buffer (buffered)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_gwritec_buf(  /* actual byte count (error != size) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* write buffer pointer */
   GIO_BIG size)      /* requested byte count */
{
   GIO_BIG n_write, n_part, n_omit, n_limit, n_need, n_want, n_flush;
   GIO_BIG new_size;
   char *new_base;
   const char *buf_next, *start;
   double time;
   struct tms tbuf;
   int saved_errno;

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

   if(! size) return 0;

   buf_next = (const char*)buf;

   /* n_need = bytes waiting to be flushed */
   n_need = fp->buf - fp->start;
   if(n_need) {
      /* write cache non-empty, continue using it */
#ifdef SNAFU_BLOCKED_TAPE_READ
   /* 9/28/97 rls, screws up blocked tape read */
      n_omit = fp->start - fp->base_p;
#else
   n_omit = 0;
#endif
      n_part = (n_omit + n_need) % fp->mod_w;
      n_want = fp->mod_w - n_part;
      if(n_need < fp->big_w) n_want += fp->big_w - n_need;
      if(n_want > size || n_want <= 0) n_want = size;

      /* n_limit = bytes available in write cache. */
      n_limit = fp->size_p - (fp->buf - fp->base_p);
      if(n_want > n_limit) n_want = n_limit;

      memcpy(fp->buf, buf_next, n_want);
      fp->buf += n_want;
      fp->count_w -= n_want;
      size -= n_want;

      if(! size && fp->count_w) {
         /* accumulate more data in the write cache */
         return n_want;
      }

      buf_next += n_want;
      n_need += n_want;

      /* need to flush data from write cache */
      start = fp->start;
      while(n_need > 0) {
         if(gio.trc_file) {
            time = times(&tbuf);
            n_write = XWRITE(fp->fd, start, n_need);
            time = (times(&tbuf) - time)/gio.clk_tck;
            fprintf(gio.trc_file, "WRITE: c_buf fd=%d nybtes=%d time(sec)=%g\n", 
                    fp->fd, (int)n_write, time);
         } else {
            n_write = XWRITE(fp->fd, start, n_need);
         }
         if(n_write <= 0) break; /* write problem */
         n_need -= n_write;
         start += n_write;
      }
      n_flush = start - fp->start;
      if(n_flush) {
         /* some data was flushed */
         fp->offset += n_flush;
         GIO_tune_write(fp, n_flush);
         fp->start = (char*)start;
      }
      if(n_need) {
         /* ouch... some data was NOT flushed ?-( */
         fp->waste = fp->size_p - (fp->start - fp->base_p);
         fp->error = errno ? errno : EIO;
         return buf_next - (const char*)buf;
      }
   }

   /* note: write cache is now empty */

   if(size >= fp->big_w) {
      /* direct write from caller's buffer */
      start = buf_next;
      while(size > 0) {
         if(gio.trc_file) {
            time = times(&tbuf);
            n_write = XWRITE(fp->fd, start, size);
            time = (times(&tbuf) - time)/gio.clk_tck;
            fprintf(gio.trc_file, "WRITE: c_buf fd=%d nybtes=%d time(sec)=%g\n", 
                    fp->fd, (int)n_write, time);
         } else {
            n_write = XWRITE(fp->fd, start, size);
         }
         if(n_write <= 0) break; /* write problem */
         size -= n_write;
         start += n_write;
      }
      n_flush = start - buf_next;
      if(n_flush) {
         /* some data was flushed */
         fp->offset += n_flush;
      }
      if(size) {
         /* ouch... some data was NOT flushed ?-( */
         fp->error = errno ? errno : EIO;
      }
      if(fp->base_p) {
         /* accumulate data in write cache */
#ifdef SNAFU_BLOCKED_TAPE_READ
         /* 9/28/97 rls, screws up blocked tape read */
         n_omit = HOST_INT6_MOD(fp->offset, fp->mod_w);
#else
         n_omit = 0;
#endif
         fp->waste = fp->size_p - n_omit;
         fp->count_w = fp->waste;
         fp->start = fp->buf = fp->base_p + n_omit;
      }
      return start - (char*)buf;
   }

   /* accumulate data in write cache */
#ifdef SNAFU_BLOCKED_TAPE_READ
   /* 9/28/97 rls, screws up blocked tape read */
   n_omit = HOST_INT6_MOD(fp->offset, fp->mod_w);
#else
   n_omit = 0;
#endif
   fp->start = fp->buf = fp->base_p + n_omit;
   fp->waste = fp->size_p - n_omit;
   fp->count_w = fp->waste;

   /* minimum size for primary cache */
   new_size = n_omit + size;
   if(new_size < fp->cache_w) new_size = fp->cache_w;
 
   if(fp->size_p < new_size) {
      /* increase size of primary cache */
      new_size = fp->mod_p * ((new_size + fp->mod_p - 1) / fp->mod_p);
      saved_errno=errno;
      new_base = (char*)GIO_MALLOC_COPY(fp, new_size);
      if(! new_base) {
	errno = fp->error = ENOMEM; 
	return buf_next - (char*)buf;
      } else {
	errno = fp->error = saved_errno;
      }
      fp->size_p = new_size;
      fp->waste = new_size - n_omit;
      fp->base_p = new_base;
      fp->start = new_base + n_omit;
   }

   memcpy(fp->start, buf_next, size);

   fp->buf = fp->base_p + n_omit + size;
   fp->count_w = fp->size_p - (n_omit + size);
   return buf_next - (char*)buf + size;
}
