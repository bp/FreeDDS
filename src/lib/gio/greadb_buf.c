static char rcsid[]="$Id: greadb_buf.c 94 2009-06-23 22:53:09Z user $";
/*
READ Biased pointer (buffered)

*/

#include "gio_sys.h"
#include "gtime.h"

int GIO_greadb_buf(   /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   GIO_BIG size,        /* requested byte count */
   GIO_ALIGN align,     /* buffer alignment */
   size_t bias)         /* alignment bias */
{
   HOST_LLONG_T n_read, n_need, n_omit, n_part, n_want, n_flush, n_skip;
   HOST_LLONG_T new_size, count_p, count_u;
   char *new_base, *buf_p, *buf_u;
   double time;
   struct tms tbuf;
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
      fp->start = fp->buf = fp->base_p;
      fp->delta = fp->count_r = fp->count_w = 0;
      fp->state = (GIO_STATE)(GIO_STATE_READ |
         (fp->state & ~ GIO_STATE_WRITE));
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

   /* additional bytes needed, to satisfy calling size */
   n_need = size - (count_u + count_p);

#ifdef SNAFU_BLOCKED_TAPE_READ
   /* 9/28/97 rls, screws up blocked tape read */
   n_omit = HOST_INT6_MOD(fp->offset, fp->mod_r);
#else
   n_omit = 0;
#endif
   if(fp->cache_r) {
      /* nominal read ahead size is greater than zero */
      n_want = (n_need > fp->cache_r - n_omit) ? n_need : fp->cache_r - n_omit;
      /* fill the cache up to the next page boundary */
      n_part = (n_omit + count_p + n_want) % fp->mod_r;
      if(n_part) n_want += fp->mod_r - n_part; /* round up */
   } else {
      /* nominal read ahead size is zero (random access) */
      n_want = n_need;
   }
 
   /* minimum size for primary cache */
   new_size = n_omit + count_p + n_want;

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
      if(count_p) {
         /* retain data already within primary cache */
         memcpy(new_base + n_omit, buf_p, count_p);
      }
      if(fp->base_p) free(fp->base_p);
      fp->size_p = new_size;
      fp->base_p = new_base;
   } else if(count_p) {
      /* retain data already within primary cache */
      memmove(fp->base_p + n_omit, buf_p, count_p);
   }

   buf_p = fp->buf = fp->start = fp->base_p + n_omit;

   while((int)n_need > 0) {
      if(gio.trc_file) {
         time = times(&tbuf);
         n_read = XREAD(fp->fd, buf_p + count_p, n_want);
         time = (times(&tbuf) - time)/gio.clk_tck;
         fprintf(gio.trc_file, "READ: b_buf fd=%d nybtes=%d time(sec)=%g\n", 
                 fp->fd, (int)n_read, time);
      } else {
         n_read = XREAD(fp->fd, buf_p + count_p, n_want); 
      }
      if((int)n_read <= 0) break; /* read problem */
      n_need -= n_read;
      n_want -= n_read;
      count_p += n_read;
   }

   if(n_read < 0)
      fp->error = errno ? errno : EIO;

   fp->waste = count_p; /* potential waste from cache read ahead */

   if(size > count_u + count_p)
      size = count_u + count_p; /* shrink */

   if(! buf_u) {
      /* required data resides in primary cache */
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
         fp->buf = buf_p + size;
         fp->count_r = count_p - size;
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
}
