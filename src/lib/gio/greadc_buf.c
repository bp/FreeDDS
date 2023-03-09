static char rcsid[]="$Id: greadc_buf.c 94 2009-06-23 22:53:09Z user $";
/*
READ copy buffer (buffered)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_greadc_buf(   /* actual byte count (error < 0) */
   GFILE *fp,         /* i/o stream */
   void *buf,         /* read buffer pointer */
   GIO_BIG size)      /* requested byte count */
{
   HOST_LLONG_T n_read, n_need, n_omit, n_part, n_want, n_flush;
   HOST_LLONG_T new_size;
   char *new_base;
   char *buf_next;
   double time;
   struct tms tbuf;
   int saved_errno;

   if(! (fp->state & GIO_STATE_READ)) {
      if(! (fp->state & GIO_STATE_OPENED)) {
         /* open standard stream (gstdin, gstdout, gstderr) */
         if(! GIO_gopen_init(fp)) {return 0;}
      }
      if(! (fp->state & GIO_STATE_OPENR)) {
         /* read not allowed on this stream */
         errno = fp->error = EACCES; return 0;
      } else if(fp->state & GIO_STATE_WRITE) {
         /* flush write cache before changing to read state */
         if(fp->gflushw(fp)) {return 0;}
      }
      /* change to read state */
      fp->start = fp->buf = fp->base_p;
      fp->delta = fp->count_r = fp->count_w = 0;
      fp->state = (GIO_STATE)(GIO_STATE_READ |
	 (fp->state & ~ GIO_STATE_WRITE));
   }

   if(! size) return 0;

   buf_next = (char*)buf;

   if(fp->count_r) {
      /* consume data from the first read cache */
      n_want = (size < fp->count_r) ? size : fp->count_r;
      memcpy(buf_next, fp->buf, n_want);
      buf_next += n_want;
      size -= n_want;
      fp->buf += n_want;
      fp->count_r -= n_want;
      if(! fp->count_r && (fp->state & GIO_STATE_UNREAD)) {
         /* unread cache empty, restore primary control */
         fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
         fp->buf = fp->buf_u;
         fp->count_r = fp->count_u;
         if(size && fp->count_r) {
            /* consume data from the primary cache */
            n_want = (size < fp->count_r) ? size : fp->count_r;
            memcpy(buf_next, fp->buf, n_want);
            buf_next += n_want;
            size -= n_want;
            fp->buf += n_want;
            fp->count_r -= n_want;
         }
      }
      if(! size) return buf_next - (char*)buf;
   }

   /* cache (unread and primary) are empty */

   n_flush = fp->buf - fp->start;

   /* flush read cache that has already been consumed */
   fp->offset += n_flush;
   GIO_tune_read(fp, n_flush);
   fp->start = fp->buf;

   if(size >= fp->big_r || ! fp->cache_r) {
      /* read data into caller's buffer (by-pass cache) */
      while(size > 0) {
         if(gio.trc_file) {
            time = times(&tbuf);
            n_read = XREAD(fp->fd, buf_next, size);
            time = (times(&tbuf) - time)/gio.clk_tck;
            fprintf(gio.trc_file, "READ: c_buf fd=%d nybtes=%d time(sec)=%g\n", 
                    fp->fd, (int)n_read, time);
         } else {
            n_read = XREAD(fp->fd, buf_next, size);
         }
         if(n_read <= 0) break; /* read problem */
         size -= n_read;
         buf_next += n_read;
         fp->offset += n_read;
      }

      if(n_read < 0)
         fp->error = errno ? errno : EIO;

      fp->waste = -1;
      if(buf_next - (char*)buf) {
         fp->state &= (GIO_STATE)(~ GIO_STATE_EOF);
      } else {
         fp->state |= GIO_STATE_EOF;
      }
      return buf_next - (char*)buf;
   }

#ifdef SNAFU_BLOCKED_TAPE_READ
   /* 9/28/97 rls, screws up blocked tape read */
   n_omit = HOST_INT6_MOD(fp->offset, fp->mod_r);
#else
   n_omit = 0;
#endif

   /* nominal read ahead size is greater than zero */
   n_want = (size > fp->cache_r - n_omit) ? size : fp->cache_r - n_omit;
   /* fill the cache up to the next page boundary */
   n_part = (n_omit + n_want) % fp->mod_r;
   if(n_part) n_want += fp->mod_r - n_part; /* round up */

   /* minimum size for primary cache */
   new_size = n_omit + n_want;

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
      fp->base_p = new_base;
   }

   fp->buf = fp->start = fp->base_p + n_omit;

   /* additional bytes needed, to satisfy calling size */
   n_need = size;

   while(n_need > 0) {
      if(gio.trc_file) {
         time = times(&tbuf);
         n_read = XREAD(fp->fd, fp->buf + fp->count_r, n_want);
         time = (times(&tbuf) - time)/gio.clk_tck;
         fprintf(gio.trc_file, "READ: c_buf fd=%d nybtes=%d time(sec)=%g\n", 
                 fp->fd, (int)n_read, time);
      } else {
         n_read = XREAD(fp->fd, fp->buf + fp->count_r, n_want);
      }
      if(n_read <= 0) break; /* read problem */
      n_need -= n_read;
      n_want -= n_read;
      fp->count_r += n_read;
   }

   if(n_read < 0)
      fp->error = errno ? errno : EIO;

   fp->waste = fp->count_r; /* potential waste from cache read ahead */

   if(fp->count_r) {
      /* consume data from the primary cache */
      if(size > fp->count_r) size = fp->count_r;
      memcpy(buf_next, fp->buf, size);
      buf_next += size;
      fp->buf += size;
      fp->count_r -= size;
   }
 
   if(buf_next - (char*)buf) {
      fp->state &= (GIO_STATE)(~ GIO_STATE_EOF);
   } else {
      fp->state |= GIO_STATE_EOF;
   }
   return buf_next - (char*)buf;
}
