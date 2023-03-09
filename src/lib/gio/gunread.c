static char rcsid[]="$Id: gunread.c 94 2009-06-23 22:53:09Z user $";
/*
UNREAD copy buffer

*/

#include "gio_sys.h"

int gunread(           /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* unread buffer pointer */
   size_t size)         /* requested byte count */
{
   GIO_BIG new_size;
   char *new_base;
   const char *buf_end;
   size_t gap_size;
   int saved_errno; 
     
   if(GIO_cfd_check(fp) < 0) return -1;

   if(fp->state & GIO_STATE_WRITE) {
      /* flush write buffer before changing to read state */
      if(fp->gflushw(fp)) {return EOF;}
   }

   if(! (fp->state & GIO_STATE_READ)) {
      /* change to read state */
      fp->start = fp->buf =
         (fp->state & GIO_STATE_MAPR) ? fp->base_m : fp->base_p;
      fp->count_r = 0;
      fp->count_w = 0; /* intercept writes mixed with reads */
      fp->state = (GIO_STATE)(GIO_STATE_READ |
         (fp->state & ~ GIO_STATE_WRITE));
   }

   if(! size) return 0;

   buf_end = (const char*)buf + size;

   if(fp->state & GIO_MASK_BUFFERED) {
      /* buffered stream */
      if(fp->state & GIO_STATE_UNREAD) {
         /* unread buffer already contains some data */
         if(buf_end == fp->buf) {
            /* unread the unread buffer, trivial */
            fp->count_r += size;
            fp->buf -= size;
         } else {
            gap_size = fp->buf - fp->base_u;
            if(gap_size < size) {
               new_size = GIO_MALLOC_MIN *
                  (1 + (fp->count_r + size - 1) / GIO_MALLOC_MIN);
	       saved_errno=errno;
               new_base = (char*)malloc(new_size);
               if(! new_base) {
		 errno = fp->error = ENOMEM; 
		 return EOF;
	       } else {
		 errno = fp->error = saved_errno;
	       }
               fp->size_u = new_size;

               /* save a copy of the existing unread data */
               gap_size = new_size - fp->count_r;
               memcpy(new_base + gap_size, fp->buf, fp->count_r);
               free(fp->base_u);
               fp->base_u = new_base;
               fp->buf = new_base + gap_size;
            }

            /* prepend more unread data */
            fp->buf -= size;
            memmove(fp->buf, buf, size);
            fp->count_r += size;
         }
      } else {
         /* unread buffer is empty */
         fp->state |= GIO_STATE_UNREAD;
         if(buf_end == fp->base_u + fp->size_u) {
            /* unread the unread buffer, trivial */
            gap_size = fp->size_u - size;
         } else {
            if(fp->size_u < size) {
               new_size = GIO_MALLOC_MIN * (1 + (size - 1) / GIO_MALLOC_MIN);
	       saved_errno=errno; 
               new_base = (char*)malloc(new_size);
               if(! new_base) {
		 errno = fp->error = ENOMEM; 
		 return EOF;
	       } else {
		 errno = fp->error = saved_errno;
	       }
               if(fp->base_u) free(fp->base_u);
               fp->base_u = new_base;
               fp->size_u = new_size;
            }
            /* save a copy of the unread data */
            gap_size = fp->size_u - size;
            memmove(fp->base_u + gap_size, buf, size);
         }
         /* swap primary and unread buffers */
         fp->count_u = fp->count_r;
         fp->count_r = size;
         fp->buf_u = fp->buf;
         fp->buf = fp->base_u + gap_size;
      }
   } else {
      /* non-buffered stream */
      if(fp->state & GIO_STATE_UNREAD) {
         /* unread buffer already contains some data */
         if(buf_end == fp->buf) {
            /* unread the unread buffer, trivial */
            fp->count_u += size;
            fp->buf -= size;
         } else {
            gap_size = fp->buf_u - fp->base_u;
            if(gap_size < size) {
               new_size = GIO_MALLOC_MIN *
                  (1 + (fp->count_u + size - 1) / GIO_MALLOC_MIN);
	       saved_errno=errno;
               new_base = (char*)malloc(new_size);
               if(! new_base) {
		 errno = fp->error = ENOMEM; 
		 return EOF;
	       } else {
		 errno = fp->error = saved_errno;
	       }
               fp->size_u = new_size;
    
               /* save a copy of the existing unread data */
               gap_size = new_size - fp->count_u;
               memcpy(new_base + gap_size, fp->buf_u, fp->count_u);
               free(fp->base_u);
               fp->base_u = new_base;
               fp->buf_u = new_base + gap_size;
            }
    
            /* prepend more unread data */
            fp->buf_u -= size;
            memmove(fp->buf_u, buf, size);
            fp->count_u += size;
         }
      } else {
         /* unread buffer is empty */
         fp->state |= GIO_STATE_UNREAD;
         if(buf_end == fp->base_u + fp->size_u) {
            /* unread the unread buffer, trivial */
            gap_size = fp->size_u - size;
         } else {
            if(fp->size_u < size) {
               new_size = GIO_MALLOC_MIN * (1 + (size - 1) / GIO_MALLOC_MIN);
	       saved_errno=errno;
               new_base = (char*)malloc(new_size);
               if(! new_base) {
		 errno = fp->error = ENOMEM; 
		 return EOF;
	       } else {
		 errno = fp->error = saved_errno;
	       }
               if(fp->base_u) free(fp->base_u);
               fp->base_u = new_base;
               fp->size_u = new_size;
            }
            /* save a copy of the unread data */
            gap_size = fp->size_u - size;
            memmove(fp->base_u + gap_size, buf, size);
         }
         /* non-buffered i/o, use unread buffers */
         fp->count_u = size;
         fp->buf_u = fp->base_u + gap_size;
      }
   }

   return size;
}
