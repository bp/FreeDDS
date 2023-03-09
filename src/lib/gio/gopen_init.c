static char rcsid[]="$Id: gopen_init.c 199 2010-11-04 17:14:10Z user $";
/*
OPEN initialization

*/

#include "gio_sys.h"

#define GIO_MAX_BLK 10

void GIO_chain(
    GFILE *fp)
{
   fp->chain = gio.open_chain;
   gio.open_chain = fp;
}

GFILE * GIO_gopen_init(   /* i/o stream (error == NULL) */
   GFILE *fp)		/* i/o stream */
{
   int mode;
   char *bufmode;

   /* state and fd *are* already initialized */

   fp->magic = ~GIO_MAGIC; /* until successfully opened */

   fp->count_r = fp->count_w = 0;
   fp->start = fp->buf = NULL;
   fp->base_m = fp->base_p = NULL;
   fp->size_m = fp->size_p = 0;
   fp->eof_size = fp->offset = 0;
   fp->size_s = fp->count_s = 0;
   fp->base_s = fp->buf_s = fp->buf_w = NULL;
   fp->cache_r = fp->cache_w = 0;
   fp->waste = fp->delta = 0;
   fp->align_p = fp->mod_p = fp->mod_r = fp->mod_w = sizeof(long);
   fp->min_r = fp->max_r = fp->min_w = fp->max_w = 0;
   fp->minsize = fp->maxsize = 0;
   fp->blksize = 1;
   fp->blkalign = 1;
   fp->error = 0;
   fp->size_u = fp->count_u = 0;
   fp->base_u = fp->buf_u = NULL;
   fp->genus = GIO_MEDIA_GENUS_UNKNOWN;

   /* initialization completed */
   fp->magic = GIO_MAGIC;
   fp->state |= GIO_STATE_OPENED;

   /* setup buffering mode */
   mode = (int)GIO_NOMINAL_BUFFERING;
   bufmode = getenv("GIO_BUFMODE");
   if (bufmode) {
      if (*bufmode == 'c') {
         mode = mode & (GIO_IO_COPY_READ | GIO_IO_COPY_WRITE);
         if (gio.verbose) {
            fprintf(gio.std_debug, "GIO_gopen_init: COPY Buffer Mode\n");
         }
      } else if (*bufmode == 'm') {
         mode = mode & (GIO_IO_MMAP_READ | GIO_IO_MMAP_WRITE);
         if (gio.verbose) {
            fprintf(gio.std_debug, "GIO_gopen_init: MMAP Buffer Mode\n");
         }
      } else if (*bufmode == 'r') {
         mode = mode & (GIO_IO_RAW_READ  | GIO_IO_RAW_WRITE);
         if (gio.verbose) {
            fprintf(gio.std_debug, "GIO_gopen_init: RAW Buffer Mode\n");
         }
      } else {
         if (gio.verbose) {
            fprintf(gio.std_debug, "GIO_gopen_init: Unknown GIO_BUFMODE=%s\n",
               bufmode);
         }
      }
   }
   gsetvbuf(fp, NULL, mode, 0);

   fp->cfd.initialized = GIO_TRUE;

   return fp;
}
