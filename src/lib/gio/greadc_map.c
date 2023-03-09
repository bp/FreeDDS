static char rcsid[]="$Id: greadc_map.c 13 2009-01-06 16:56:43Z user $";
/*
READ copy buffer (memory mapped)

*/

#include "gio_sys.h"

int GIO_greadc_map(   /* actual byte count (error < 0) */
   GFILE *fp,         /* i/o stream */
   void *buf,         /* read buffer pointer */
   GIO_BIG size)      /* requested byte count */
{
   #ifdef GIO_MMAP_IS_SUPPORTED
      HOST_LLONG_T n_omit, n_part, n_flush, n_want;
      HOST_LLONG_T new_size;
      char *new_base, *buf_next;
      HOST_INT6_T n_huge, new_offset;
      struct GIO_STAT stat_buf;

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
         fp->start = fp->buf = NULL;
         fp->delta = fp->count_r = fp->count_w = 0;
	 fp->state = (GIO_STATE)(GIO_STATE_READ |
	    (fp->state & ~ GIO_STATE_WRITE));
         /* retrieve the current file size */
         if(XFSTAT(fp->fd, &stat_buf) || stat_buf.st_size < 0)
         {
            fp->error = errno ? errno : EIO;
            return 0;
         }
         fp->eof_size = stat_buf.st_size;
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
            /* unread cache empty, restore mmap control */
            fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
            fp->buf = fp->buf_u;
            fp->count_r = fp->count_u;
            if(size && fp->count_r) {
               /* consume data from the mmap cache */
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

      /* cache (unread and mmap) are empty */

      n_flush = fp->buf - fp->start;

      /* flush read cache that has already been consumed */
      fp->offset += n_flush;
      GIO_tune_read(fp, n_flush);
      fp->start = fp->buf;
    
      n_omit = HOST_INT6_MOD(fp->offset, fp->mod_r);
      new_offset = fp->offset - n_omit;

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

      /* truncate mapping, if it exceeds eof */
      /* ??? dynamic eof_size ??? */
      n_huge = new_size + new_offset - fp->eof_size;
      if(n_huge > 0) {
         /* requested new_size exceeds eof */
         fp->state |= GIO_STATE_KEOF;
         new_size -= n_huge;
      }

      if(new_size) {
         /* unmap old cache */
         if(fp->base_m && munmap(fp->base_m, fp->size_m)) {
            /* error, can't un-map current memory pages ? */
            fp->error = errno ? errno : EIO;
            return buf_next - (char*)buf;
         }

         /* mmap the stream, from new_offset for new_size bytes */
         new_base = mmap(NULL, new_size, GIO_MMAP_PROT, GIO_MMAP_FLAGS,
            fp->fd, (off_t)new_offset);
         if((void*)-1l == new_base) {
            /* error, can't mmap this stream into a memory page ? */
            fp->error = errno ? errno : EIO;
            fp->buf = fp->start = fp->base_m = NULL;
            fp->count_r = fp->size_m = 0;
         } else {
            /* cleanup after read mmap */
            madvise(new_base, new_size, MADV_WILLNEED);
            fp->count_r = new_size - n_omit;
            fp->waste = fp->count_r; /* potential waste from cache read ahead */
            fp->buf = new_base + n_omit;
            fp->offset = new_offset;
            fp->start = fp->base_m = new_base;
            fp->size_m = new_size;
         }
      } else {
         /* current position is at EOF. */
         fp->offset -= n_flush;
         fp->start = fp->buf - n_flush;
         fp->waste = n_flush;
      }


      if(fp->count_r) {
         /* consume data from the mmap cache */
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

   #else /* GIO_MMAP_IS_SUPPORTED */
      return 0;
   #endif /* GIO_MMAP_IS_SUPPORTED */
}
