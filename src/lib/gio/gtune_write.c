static char rcsid[]="$Id: gtune_write.c 13 2009-01-06 16:56:43Z user $";
/*
TUNE WRITE cache
   fp->waste is the number of bytes cached by GIO (potential waste).
   GIO_tune_write is called by gflushw_* (prior to seek or read),
   and by gwrite?_* (prior to writing more data).

*/

#include "gio_sys.h"

void GIO_tune_write(
   GFILE *fp,           /* i/o stream */
   GIO_BIG n_flush)     /* number of bytes actually written by the API */
{
   GIO_BIG cache_old;

   if(GIO_MIN_WRITE * fp->waste < 100 * n_flush) {
      /* small waste */
      if(fp->cache_w < fp->max_w) {
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

         cache_old = fp->cache_w;
         fp->cache_w += fp->delta * fp->mod_w;

         if(fp->cache_w > fp->max_w) {
            /* maximum size, decrease potential decay magnitude */
            fp->cache_w = fp->max_w;
            fp->delta = (fp->delta - 1) / 2;
         }
         if (gio.verbose && fp->cache_w != cache_old) {
            fprintf(gio.std_debug, "gio: write cache increased from %d to %d\n\tfd=%d name=%s\n",
               (int)cache_old, (int)fp->cache_w, fp->fd, fp->cfd.name);
         }
      }
   } else if(fp->waste > 0) {
      /* large waste */
      if(fp->cache_w > fp->min_w) {
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

         cache_old = fp->cache_w;
         fp->cache_w += fp->delta * fp->mod_w;

         if(fp->cache_w < fp->min_w) {
            /* minimum size, decrease potential growth magnitude */
            fp->cache_w = fp->min_w;
            fp->delta = (fp->delta + 1) / 2;
         }
         if (gio.verbose && fp->cache_w != cache_old) {
            fprintf(gio.std_debug, "gio: write cache decreased from %d to %d\n\tfd=%d name=%s\n",
               (int)cache_old, (int)fp->cache_w, fp->fd, fp->cfd.name);
         }
      }
   }
   return;
}
