static char rcsid[]="$Id: gtune_read.c 13 2009-01-06 16:56:43Z user $";
/*
TUNE READ cache size.
   fp->waste is the number of bytes cached by GIO (potential waste).
   GIO_tune_read is called by gflushr_* (prior to seek or write),
   and by gread?_* (prior to reading more data).
*/

#include "gio_sys.h"

void GIO_tune_read(
   GFILE *fp,           /* i/o stream */
   GIO_BIG n_flush)     /* number of bytes actually read by the API */
{
   GIO_BIG cache_old;

   if(GIO_MIN_READ * fp->waste < 100 * n_flush) {
      /* small waste */
      if(fp->cache_r < fp->max_r) {
         /* bigger is better */
         if(fp->delta > 0) {
            /* accelerate cache growth */
            fp->delta *= 2;
         } else if(fp->delta < 0) {
            /* transition from decay to growth, at half the magnitude */
            fp->delta /= -2;
         } else {
            /* slowly grow cache from stable size */
            fp->delta = +1;
         }

         cache_old = fp->cache_r;
         fp->cache_r += fp->delta * fp->mod_r;

         if(fp->cache_r > fp->max_r) {
            /* maximum size, decrease potential decay magnitude */
            fp->cache_r = fp->max_r;
            fp->delta = (fp->delta - 1) / 2;
         }
         if (gio.verbose && fp->cache_r != cache_old) {
            fprintf(gio.std_debug, "gio: read cache increased from %d to %d\n\tfd=%d name=%s\n",
               (int)cache_old, (int)fp->cache_r, fp->fd, fp->cfd.name);
         }
      }
   } else if(fp->waste > 0) {
      /* large waste */
      if(fp->cache_r > fp->min_r) {
         /* smaller is smarter */
         if(fp->delta < 0) {
            /* accelerate cache decay */
            fp->delta *= 2;
         } else if(fp->delta > 0) {
            /* transition from growth to decay, at half the magnitude */
            fp->delta /= -2;
         } else {
            /* slowly decay cache from stable size */
            fp->delta = -1;
         }

         cache_old = fp->cache_r;
         fp->cache_r += fp->delta * fp->mod_r;

         if(fp->cache_r < fp->min_r) {
            /* minimum size, decrease potential growth magnitude */
            fp->cache_r = fp->min_r;
            fp->delta = (fp->delta + 1) / 2;
         }
         if (gio.verbose && fp->cache_r != cache_old) {
            fprintf(gio.std_debug, "gio: read cache decreased from %d to %d\n\tfd=%d name=%s\n",
               (int)cache_old, (int)fp->cache_r, fp->fd, fp->cfd.name);
         }
      }
   }
   return;
}
