static char rcsid[]="$Id: greadc_raw.c 94 2009-06-23 22:53:09Z user $";
/*
  READ copy buffer (raw/direct)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_greadc_raw(   /* actual byte count (error < 0) */
                   GFILE *fp,         /* i/o stream */
                   void *buf,         /* read buffer pointer */
                   GIO_BIG size)      /* requested byte count */
{
#ifdef GIO_RAWIO_IS_SUPPORTED
   HOST_LLONG_T n_read, n_want, n_part, n_flush;
   HOST_LLONG_T n_limit, n_omit;
   HOST_LLONG_T new_size;
   char *new_base, *buf_next;
   double time;
   struct tms tbuf;
   HOST_INT6_T new_offset;
   int saved_errno;
#ifndef GIO_RAWIO_HONORS_KERNEL_EOF
   HOST_INT6_T n_huge;
   HOST_LLONG_T n_chop;
   struct GIO_STAT stat_buf;
#endif /* GIO_RAWIO_HONORS_KERNEL_EOF */

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
#ifndef GIO_RAWIO_HONORS_KERNEL_EOF
      /* retrieve the current file size */
      if(XFSTAT(fp->fd, &stat_buf) || stat_buf.st_size < 0)
         {
            fp->error = errno ? errno : EIO;
            return 0;
         }
      fp->eof_size = stat_buf.st_size;
#endif /* GIO_RAWIO_HONORS_KERNEL_EOF */
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
    
   new_offset = fp->offset;
   n_omit = HOST_INT6_MOD(fp->offset, fp->mod_r);

   if(n_omit) {
      /* align offset to a page boundary within stream */
      new_offset -= n_omit;
      if(GIO_SEEK(fp, new_offset, SEEK_SET) < 0) {
         fp->error = errno ? errno : EIO;
         return buf_next - (char*)buf;
      }
      fp->offset = new_offset;
   }

   if(size + n_omit > fp->cache_r) {
      /* fill the cache up to the next page boundary */
      n_want = size + n_omit;
      n_part = n_want % fp->mod_r;
      if(n_part) n_want += fp->mod_r - n_part; /* round up */
   } else {
      n_want = fp->cache_r;
   }

   /* minimum size for primary cache */
   new_size = n_want;
    
#ifndef GIO_RAWIO_HONORS_KERNEL_EOF
   if(fp->state & GIO_STATE_KEOF) {
      /* retrieve the current file size */
      if(XFSTAT(fp->fd, &stat_buf) || stat_buf.st_size < 0)
         {
            fp->error = errno ? errno : EIO;
            return 0;
         }
      fp->eof_size = stat_buf.st_size;
      fp->state &= (GIO_STATE)(~ GIO_STATE_KEOF);
   }

   /* truncate raw read, if it exceeds eof */
   /* ??? dynamic eof_size ??? */
   n_huge = new_size + new_offset - fp->eof_size;
   if(n_huge > 0) {
      /* requested new_size exceeds eof */
      fp->state |= GIO_STATE_KEOF;
      n_chop = n_huge;
   } else {
      n_chop = 0;
   }
#endif /* GIO_RAWIO_HONORS_KERNEL_EOF */

   if(fp->size_p < new_size) {
      /* increase size of primary cache */
      new_size = fp->mod_p * ((new_size + fp->mod_p - 1) / fp->mod_p);
      saved_errno=errno;
      new_base = (char*)GIO_MALLOC_RAWIO(fp, new_size);
      if(! new_base) {
	errno = fp->error = ENOMEM; 
	return buf_next - (char*)buf;
      } else {
	errno = fp->error = saved_errno;
      }
      if(fp->base_p) free(fp->base_p);
      fp->size_p = new_size;
      fp->base_p = new_base;
   }

   fp->start = fp->base_p;
   fp->buf = fp->base_p + n_omit;
   fp->count_r = - n_omit;

   while(size > fp->count_r) {
      n_limit = (n_want < fp->max_r) ? n_want : fp->max_r;
      if(gio.trc_file) {
         time = times(&tbuf);
         n_read = XREAD(fp->fd, fp->buf + fp->count_r, n_limit);
         time = (times(&tbuf) - time)/gio.clk_tck;
         fprintf(gio.trc_file, "READ: c_raw fd=%d nybtes=%d time(sec)=%g\n", 
                 fp->fd, (int)n_read, time);
      } else {
         n_read = XREAD(fp->fd, fp->buf + fp->count_r, n_limit);
      }
      if(n_read <= 0) break; /* read problem */
      n_want -= n_read;
      fp->count_r += n_read;
   }

#ifdef GIO_RAWIO_HONORS_KERNEL_EOF
   if(n_read < 0)
      fp->error = errno ? errno : EIO;
#else /* GIO_RAWIO_HONORS_KERNEL_EOF */
   if(n_read < 0) fp->error = errno ? errno : EIO;
   if(n_chop > n_want) {
      fp->state |= GIO_STATE_KEOF;
      fp->count_r -= n_chop - n_want;
   }
#endif /* GIO_RAWIO_HONORS_KERNEL_EOF */

   if(fp->count_r < 0) {
      /* truncated file, can't access what we already had ! */
      assert(! "raw read beyond eof (truncated file)");
      fp->count_r = 0; /* how can we continue like this ? */
   }

   /* potential waste from cache read ahead */
   fp->waste = n_omit + fp->count_r;

   if(fp->count_r) {
      /* consume data from the primary cache */
      n_want = (size < fp->count_r) ? size : fp->count_r;
      memcpy(buf_next, fp->buf, n_want);
      buf_next += n_want;
      fp->buf += n_want;
      fp->count_r -= n_want;
   }

   if(buf_next - (char*)buf) {
      fp->state &= (GIO_STATE)(~ GIO_STATE_EOF);
   } else {
      fp->state |= GIO_STATE_EOF;
   }
   return buf_next - (char*)buf;

#else    /* GIO_RAWIO_IS_SUPPORTED */
   return 0;
#endif /* GIO_RAWIO_IS_SUPPORTED */
}
