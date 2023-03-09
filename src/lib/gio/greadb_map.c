static char rcsid[]="$Id: greadb_map.c 94 2009-06-23 22:53:09Z user $";
/*
READ Biased pointer (memory mapped)

*/

#include "gio_sys.h"

int GIO_greadb_map(   /* actual byte count (error < 0) */
   GFILE *fp,         /* i/o stream */
   const void **buf,  /* read buffer pointer to pointer */
   GIO_BIG size,      /* requested byte count */
   GIO_ALIGN align,   /* buffer alignment */
   size_t bias)       /* alignment bias */
{
   #ifdef GIO_MMAP_IS_SUPPORTED
      HOST_LLONG_T n_keep, n_need, n_want, n_omit, n_part, n_skip, n_flush;
      HOST_LLONG_T new_size, count_m, count_u;
      char *new_base, *buf_m, *buf_u;
      HOST_INT6_T n_huge, new_offset;
      struct GIO_STAT stat_buf;
      int saved_errno;

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
         fp->start = fp->buf = NULL;
         fp->delta = fp->count_r = fp->count_w = 0;
	 fp->state = (GIO_STATE)(GIO_STATE_READ |
	    (fp->state & ~ GIO_STATE_WRITE));
         /* retrieve the current file size */
         if(XFSTAT(fp->fd, &stat_buf) || stat_buf.st_size < 0)
         {
            fp->error = errno ? errno : EIO;
            *buf = NULL;
            return 0;
         }
         fp->eof_size = stat_buf.st_size;
      }

      if(! size) {*buf = NULL; return 0;}

      if(fp->count_r >= size) {
         /* all data available in the first cache */
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
            /* copy data into secondary cache */
            GIO_PTR_SKIP(n_skip, fp->base_s, align, bias);
            *buf = fp->base_s + n_skip;
            memcpy(fp->base_s + n_skip, fp->buf, size);
         }
         fp->buf += size;
         fp->count_r -= size;
         if(! fp->count_r && (fp->state & GIO_STATE_UNREAD)) {
            /* unread cache empty, restore mmap control */
            fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
            fp->buf = fp->buf_u;
            fp->count_r = fp->count_u;
         }
         return size;
      }

      if((fp->state & GIO_STATE_UNREAD) && fp->count_r + fp->count_u >= size) {
         /* required data crosses cache (unread + mmap) */
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

         /* assemble requested data (unread + mmap) in secondary cache */ 
         GIO_PTR_SKIP(n_skip, fp->base_s, align, bias);
         *buf = fp->base_s + n_skip;
         memcpy(fp->base_s + n_skip, fp->buf, fp->count_r);
         n_need = size - fp->count_r;
         memcpy(fp->base_s + n_skip + fp->count_r, fp->buf_u, n_need);

         /* unread cache empty, restore mmap control */
         fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
         fp->buf = fp->buf_u + n_need;
         fp->count_r = fp->count_u - n_need;
         return size;
      }

      /* discriminate between unread and mmap cache */

      if(fp->state & GIO_STATE_UNREAD) {
         /* unread cache non-empty */
         buf_u = fp->buf;
         count_u = fp->count_r;
         buf_m = fp->buf_u;
         count_m = fp->count_u;
      } else {
         /* unread cache empty */
         buf_u = NULL;
         count_u = 0;
         buf_m = fp->buf;
         count_m = fp->count_r;
      }

      /* read more data from the kernel */
      n_flush = buf_m - fp->start;

      /* flush read cache that has already been consumed */
      fp->offset += n_flush;
      n_flush += count_m; /* not wasted */
      GIO_tune_read(fp, n_flush);
      fp->start = buf_m;

      n_omit = HOST_INT6_MOD(fp->offset, fp->mod_r);
      new_offset = fp->offset - n_omit;
 
      /* additional bytes needed, to satisfy calling size */
      n_keep = fp->mod_r * ((n_omit + count_m) / fp->mod_r);
      n_need = size - (count_u + n_keep);
 
      /* nominal read ahead size is greater than zero */
      if((n_keep ? n_need : n_need + n_omit) > fp->cache_r) {
         /* fill the cache up to the next page boundary */
         n_want = n_omit + size - n_keep;
         n_part = (n_omit + size) % fp->mod_r;
         if(n_part) n_want += fp->mod_r - n_part; /* round up */
      } else {
         n_want = fp->cache_r;
      }
 
      /* minimum size for primary cache */
      new_size = n_keep + n_want;
    
      if(fp->state & GIO_STATE_KEOF) {
         /* retrieve the current file size */
         if(XFSTAT(fp->fd, &stat_buf) || stat_buf.st_size < 0)
         {
            fp->error = errno ? errno : EIO;
            *buf = NULL;
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
            *buf = NULL;
            return 0;
         }

         /* mmap the stream, from new_offset for new_size bytes */
         new_base = mmap(NULL, new_size, GIO_MMAP_PROT, GIO_MMAP_FLAGS,
            fp->fd, (off_t)new_offset);
         if((void*)-1l == new_base) {
            /* error, can't mmap this stream into a memory page ? */
            fp->error = errno ? errno : EIO;
            new_offset += n_omit;
            fp->offset = new_offset;
            buf_m = fp->start = fp->base_m = NULL;
            count_m = fp->waste = fp->size_m = 0;
         } else {
            /* mmap successful.   cleanup after read */
            #ifdef GIO_MADVISE_WILLNEED
               madvise(new_base, new_size, MADV_WILLNEED);
            #endif /* GIO_MADVISE_WILLNEED */
            count_m = new_size - n_omit;
            fp->waste = count_m; /* potential waste from cache read ahead */
            buf_m = new_base + n_omit;
            fp->offset = new_offset;
            fp->start = fp->base_m = new_base;
            fp->size_m = new_size;
         }
      } else {
         /* current position is at EOF. */
         fp->offset -= n_flush;
         fp->start = buf_m - n_flush;
         fp->waste = n_flush;
      }

      if(size > count_u + count_m)
         size = count_u + count_m; /* shrink */

      if(! buf_u) {
         /* required data resides in mmap cache */
         if(size) {
            if(GIO_PTR_BIASED(buf_m, align, bias)) {
               /* cache address satisfies biased alignment */
               *buf = buf_m;
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
               /* copy data into secondary cache */
               GIO_PTR_SKIP(n_skip, fp->base_s, align, bias);
               *buf = fp->base_s + n_skip;
               memcpy(fp->base_s + n_skip, buf_m, size);
            }
            fp->buf = buf_m + size;
            fp->count_r = count_m - size;
            fp->state &= (GIO_STATE)(~ GIO_STATE_EOF);
            return size;
         }
         fp->buf = buf_m;
         fp->count_r = 0;
         fp->state |= GIO_STATE_EOF;
         *buf = NULL;
         return 0;
      }

      /* required data crosses cache (unread + mmap) */

      new_size = GIO_MALLOC_MIN *
         (1 + (size + align - 1) / GIO_MALLOC_MIN);
      if(fp->size_s < new_size) {
         /* increase size of secondary cache */
	saved_errno=errno;
	new_base = (char*)malloc(new_size);
	if(! new_base) {
	  fp->count_u = count_m;
	  fp->buf_u = buf_m;
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

      /* assemble requested data (unread + mmap) in secondary cache */
      GIO_PTR_SKIP(n_skip, fp->base_s, align, bias);
      *buf = fp->base_s + n_skip;
      memcpy(fp->base_s + n_skip, buf_u, count_u);
      fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
      n_need = size - count_u;
      memcpy(fp->base_s + n_skip + count_u, buf_m, n_need);
      fp->buf = buf_m + n_need;
      fp->count_r = count_m - n_need;
      fp->state &= (GIO_STATE)(~ GIO_STATE_EOF);
      return size;

   #else /* GIO_MMAP_IS_SUPPORTED */
      *buf = NULL;
      return 0;
   #endif /* GIO_MMAP_IS_SUPPORTED */
}
