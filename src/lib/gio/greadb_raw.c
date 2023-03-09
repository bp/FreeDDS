static char rcsid[]="$Id: greadb_raw.c 94 2009-06-23 22:53:09Z user $";
/*
READ Biased pointer (raw/direct)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_greadb_raw(   /* actual byte count (error < 0) */
   GFILE *fp,         /* i/o stream */
   const void **buf,  /* read buffer pointer to pointer */
   GIO_BIG size,      /* requested byte count */
   GIO_ALIGN align,   /* buffer alignment */
   size_t bias)       /* alignment bias */
{
   #ifdef GIO_RAWIO_IS_SUPPORTED
      HOST_LLONG_T n_read, n_need, n_want, n_part, n_skip, n_flush;
      HOST_LLONG_T n_limit, n_omit, n_keep;
      HOST_LLONG_T new_size, count_p, count_u;
      char *new_base, *buf_p, *buf_u;
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
         fp->delta = fp->count_r = fp->count_w = 0;
	 fp->state = (GIO_STATE)(GIO_STATE_READ |
	    (fp->state & ~ GIO_STATE_WRITE));
         #ifndef GIO_RAWIO_HONORS_KERNEL_EOF
            /* retrieve the current file size */
            if(XFSTAT(fp->fd, &stat_buf) || stat_buf.st_size < 0)
            {
               fp->error = errno ? errno : EIO;
               *buf = NULL;
               return 0;
            }
            fp->eof_size = stat_buf.st_size;
         #endif /* GIO_RAWIO_HONORS_KERNEL_EOF */
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
            /* unread cache empty, restore primary control */
            fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
            fp->buf = fp->buf_u;
            fp->count_r = fp->count_u;
         }
         return size;
      }

      if((fp->state & GIO_STATE_UNREAD) && fp->count_r + fp->count_u >= size) {
         /* required data crosses cache (unread + primary) */
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

         /* assemble requested data (unread + primary) in secondary cache */ 
         GIO_PTR_SKIP(n_skip, fp->base_s, align, bias);
         *buf = fp->base_s + n_skip;
         memcpy(fp->base_s + n_skip, fp->buf, fp->count_r);
         n_need = size - fp->count_r;
         memcpy(fp->base_s + n_skip + fp->count_r, fp->buf_u, n_need);

         /* unread cache empty, restore primary control */
         fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
         fp->buf = fp->buf_u + n_need;
         fp->count_r = fp->count_u - n_need;
         return size;
      }

      /* discriminate between unread and primary cache */

      if(fp->state & GIO_STATE_UNREAD) {
         /* unread cache non-empty */
         buf_u = fp->buf;
         count_u = fp->count_r;
         buf_p = fp->buf_u;
         count_p = fp->count_u;
      } else {
         /* unread cache empty */
         buf_u = NULL;
         count_u = 0;
         buf_p = fp->buf;
         count_p = fp->count_r;
      }

      /* read more data from the kernel */
      n_flush = buf_p - fp->start;

      /* flush read cache that has already been consumed */
      fp->offset += n_flush;
      n_flush += count_p; /* not wasted */
      GIO_tune_read(fp, n_flush);
      fp->start = buf_p;
    
      n_omit = HOST_INT6_MOD(fp->offset, fp->mod_r);
      new_offset = fp->offset - n_omit;

      /* additional bytes needed, to satisfy calling size */
      n_keep = fp->mod_r * ((n_omit + count_p) / fp->mod_r);
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

      #ifndef GIO_RAWIO_HONORS_KERNEL_EOF
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
         /* truncate raw read, if it exceeds eof */
         /* ??? dynamic eof_size ??? */
         n_huge = new_size + new_offset - fp->eof_size;
         if(n_huge > 0) {
            /* requested new_size exceeds eof */
            fp->state |= GIO_STATE_KEOF;
            n_chop = n_huge;
         }
      #endif /* GIO_RAWIO_HONORS_KERNEL_EOF */


      if(fp->size_p < new_size) {
         /* increase size of primary cache */
         new_size = fp->mod_p * ((new_size + fp->mod_p - 1) / fp->mod_p);
	 saved_errno=errno;
         new_base = (char*)GIO_MALLOC_RAWIO(fp, new_size);
         if(! new_base) {
	   errno = fp->error = ENOMEM; 
	   *buf = NULL; 
	   return 0;
	 } else {
	   errno = fp->error = saved_errno;
	 }
         if(n_keep) {
            /* retain data already within primary cache */
            memcpy(new_base, buf_p - n_omit, n_keep);
         }
         if(fp->base_p) free(fp->base_p);
         fp->size_p = new_size;
         fp->base_p = new_base;
      } else if(n_keep && fp->base_p != buf_p - n_omit) {
         /* retain data already within primary cache */
         memmove(fp->base_p, buf_p - n_omit, n_keep);
      }

      fp->start = fp->base_p;
      buf_p = fp->base_p + n_omit;
      fp->offset = new_offset;

      if(n_omit + count_p != n_keep) {
         /* seek required to correct stream offset */
         new_offset += n_keep;
         if(GIO_SEEK(fp, new_offset, SEEK_SET) < 0) {
            fp->error = errno ? errno : EIO;
            *buf = NULL;
            return 0;
         }
      }

      count_p = n_keep - n_omit;

      while(size > count_u + count_p) {
         n_limit = (n_want < fp->max_r) ? n_want : fp->max_r;
         if(gio.trc_file) {
            time = times(&tbuf);
            n_read = XREAD(fp->fd, buf_p + count_p, n_limit);
            time = (times(&tbuf) - time)/gio.clk_tck;
            fprintf(gio.trc_file, "READ: b_raw fd=%d nybtes=%d time(sec)=%g\n", 
                    fp->fd, (int)n_read, time);
         } else {
            n_read = XREAD(fp->fd, buf_p + count_p, n_limit);
         }
         if(n_read <= 0) break; /* read problem */
         n_want -= n_read;
         count_p += n_read;
      }

      #ifdef GIO_RAWIO_HONORS_KERNEL_EOF
         if(n_read < 0)
            fp->error = errno ? errno : EIO;
      #else /* GIO_RAWIO_HONORS_KERNEL_EOF */
         if(n_read < 0) fp->error = errno ? errno : EIO;
         if(n_chop > n_want) {
            fp->state |= GIO_STATE_KEOF;
            count_p -= n_chop - n_want;
         }
      #endif /* GIO_RAWIO_HONORS_KERNEL_EOF */


      if(count_p < 0) {
         /* truncated file, can't access what we already had ! */
         assert(! "raw read beyond eof (truncated file)");
         count_p = 0; /* how can we continue like this ? */
      }

      /* potential waste from cache read ahead */
      fp->waste = n_omit + count_p;

      if(size > count_u + count_p)
         size = count_u + count_p; /* shrink */

      if(! buf_u) {
         /* required data resides in primary cache */
         fp->buf = buf_p + size;
         fp->count_r = count_p - size;
         if(size) {
            if(GIO_PTR_BIASED(buf_p, align, bias)) {
               /* cache address satisfies biased alignment */
               *buf = buf_p;
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
               memcpy(fp->base_s + n_skip, buf_p, size);
            }
            fp->state &= (GIO_STATE)(~ GIO_STATE_EOF);
            return size;
         }
         fp->state |= GIO_STATE_EOF;
         *buf = NULL;
         return 0;
      }

      /* required data crosses cache (unread + primary) */

      new_size = GIO_MALLOC_MIN *
         (1 + (size + align - 1) / GIO_MALLOC_MIN);
      if(fp->size_s < new_size) {
         /* increase size of secondary cache */
	saved_errno=errno;
	new_base = (char*)malloc(new_size);
	if(! new_base) {
	  fp->count_u = count_p;
	  fp->buf_u = buf_p;
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

      /* assemble requested data (unread + primary) in secondary cache */
      GIO_PTR_SKIP(n_skip, fp->base_s, align, bias);
      *buf = fp->base_s + n_skip;
      memcpy(fp->base_s + n_skip, buf_u, count_u);
      fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
      n_need = size - count_u;
      memcpy(fp->base_s + n_skip + count_u, buf_p, n_need);
      fp->buf = buf_p + n_need;
      fp->count_r = count_p - n_need;
      fp->state &= (GIO_STATE)(~ GIO_STATE_EOF);
      return size;

   #else    /* GIO_RAWIO_IS_SUPPORTED */
      *buf = NULL;
      return 0;
   #endif    /* GIO_RAWIO_IS_SUPPORTED */
}
