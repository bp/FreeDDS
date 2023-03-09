static char rcsid[]="$Id: gsetvbuf.c 305 2016-11-07 21:14:31Z user $";
/*
SETVBUF

*/

#include "gio_sys.h"

/*** NOTE: special host dependencies... (SEE ALSO gio_config.h)

   The following cache constraints are host dependent.
***/


int gsetvbuf(           /* status (error != 0) */
   GFILE *fp,           /* i/o stream */
   void *buf,           /* buffer pointer (ignored) */
   int mode,            /* _IOFBF, _IOLBF, _IONBF, ... */
   size_t size)         /* block size */
{
   HOST_LLONG_T block_size, page_size;
   int mode_buffer, mode_read, mode_write;
   HOST_INT6_T offset;

   if(GIO_cfd_check(fp) < 0) return -1;

   /* initialize gstdin, gstdout, or gstderr, prior to access */
   if(! (fp->state & GIO_STATE_OPENED) &&
      ! GIO_gopen_init(fp)) return EOF;

   if(fp->state & (GIO_STATE_READ | GIO_STATE_WRITE)) {
      /* too late to change buffering scheme */
      errno = EINVAL;
      return EOF;
   }

   if(GIO_setvbuf_mode(fp, &mode_buffer, &mode_read, &mode_write, mode)) {
      /* mode provided by API is not valid. */
      return EOF;
   }

   if (gio.blk_size < 0) {
      mode = _IONON_CACHE; /* user turned off buffering */
      if (gio.verbose) {
       fprintf(stderr, "WARNING gsetvbuf: _IONON_CACHE, no I/O cache used for fd=%d %s\n",
              fp->fd, fp->cfd.name);
      }
   }

   GIO_setvbuf_stat(fp);

   /* current stream offset, as known by kernel */
   offset = GIO_SEEK(fp, 0, SEEK_CUR);

   if(offset < 0) {
      /* negative == seek error (not allowed ?) */
      fp->state &= (GIO_STATE) ~ (GIO_STATE_SEEK);
   } else {
      /* update current offset */
      fp->state |= GIO_STATE_SEEK;
      fp->offset = offset;
   }

   /**********************************************************************/
   /* set state flags as specified by mode (buffer and kernel semantics) */
   /**********************************************************************/

   fp->align_p = sizeof(long);

   fp->state &= (GIO_STATE) ~
      (GIO_STATE_FULLBUF | GIO_STATE_LINEBUF |
       GIO_MASK_COPIED | GIO_MASK_MAPPED | GIO_MASK_RAWIO);

   /***********************************/
   /* initialize BUFFERING parameters */
   /***********************************/

   switch(mode_buffer) {
   case _IOFBF: /* full buffering */
   case _IOLBF: /* line buffering */
   default:
      /***********************************/
      /* Default, FULL or LINE buffering. */
      /***********************************/

      if(_IOLBF == mode_buffer)
         fp->state |= GIO_STATE_LINEBUF;
      else
         fp->state |= GIO_STATE_FULLBUF;

      /************************************/
      /* Initialize READ mode parameters. */
      /************************************/
      switch(mode_read) {
      #ifdef GIO_MMAP_IS_SUPPORTED
         case GIO_IO_MMAP_READ:
            fp->state |= GIO_STATE_MAPR;

            fp->mod_r = gio.page_size;

            page_size = size ? fp->mod_r *
                  ((size + fp->mod_r - 1) / fp->mod_r):
               fp->mod_r;

            if(mode & _IONON_CACHE) {
               /* development only, i.e. NOT production usage */
               fp->cache_r = fp->min_r = fp->max_r = page_size;
               fp->big_r = INT_MAX;
            } else if(mode & _IONON_TUNED) {
               /* development only, i.e. NOT production usage */
               fp->cache_r = fp->min_r = fp->max_r = page_size;
               fp->big_r = INT_MAX;
            } else if(size) {
               fp->min_r = fp->mod_r;
               fp->max_r = fp->cache_r = page_size;
               fp->big_r = page_size;
            } else {
               #ifdef HOST_Sun
                  fp->min_r   = page_size;
                  fp->max_r   = 16 * page_size; /* 15 May 1998 @ 8k ptr */
                  fp->big_r   = 16 * page_size; /* 15 May 1998 @ 8k ptr */
                  fp->cache_r = fp->big_r;
               #elif HOST_HP
                  fp->min_r   = page_size;
                  fp->max_r   = 2 * page_size;
                  fp->big_r   = 1 * page_size;
                  fp->cache_r = fp->big_r;
               #elif HOST_SGI
                  fp->min_r   = page_size;
                  fp->max_r   = 4 * page_size;
                  fp->big_r   = 2 * page_size;
                  fp->cache_r = fp->big_r;
               #else
                  fp->min_r   = page_size;
                  fp->max_r   = 8 * page_size;
                  fp->big_r   = 2 * page_size;
                  fp->cache_r = fp->big_r;
               #endif
            }
            fp->greadm = &GIO_int_defer;
            fp->greada = &GIO_int_defer;
            fp->greadb = &GIO_greadb_map;
            fp->greadc = &GIO_greadc_map;
            fp->gunread = &gunread;
            fp->gflushr = &GIO_gflushr_map;
            break;
      #endif /* GIO_MMAP_IS_SUPPORTED */
      #ifdef GIO_RAWIO_IS_SUPPORTED
         case GIO_IO_RAW_READ:
            fp->state |= GIO_STATE_RAWR;

            if(fp->align_p < fp->blkalign) fp->align_p = fp->blkalign;

            fp->mod_r = fp->minsize;

            block_size = size ? fp->mod_r *
                  ((size + fp->mod_r - 1) / fp->mod_r):
               fp->mod_r;

            if(mode & _IONON_CACHE) {
               /* development only, i.e. NOT production usage */
               fp->cache_r = fp->min_r = fp->max_r = block_size;
               fp->big_r = INT_MAX;
            } else if(mode & _IONON_TUNED) {
               /* development only, i.e. NOT production usage */
               fp->cache_r = fp->min_r = fp->max_r = block_size;
               fp->big_r = INT_MAX;
            } else if(size) {
               fp->min_r = fp->mod_r;
               fp->max_r = fp->cache_r = block_size;
               fp->big_r = block_size;
            } else {
               #ifdef HOST_SGI
                  fp->min_r   = block_size;
                  fp->max_r   = 64 * block_size;
                  fp->big_r   = 8 * block_size;
                  fp->cache_r = fp->big_r;
               #else
                  fp->min_r   = block_size;
                  fp->max_r   = 16 * block_size;
                  fp->big_r   = 2 * block_size;
                  fp->cache_r = fp->big_r;
               #endif
            }
            if(fp->min_r < fp->minsize) fp->min_r = fp->minsize;
            if(fp->max_r > fp->maxsize) fp->max_r = fp->maxsize;
            if(fp->big_r < fp->minsize) fp->big_r = fp->minsize;
            if(fp->big_r > fp->maxsize) fp->big_r = fp->maxsize;

            fp->greadm = &GIO_int_defer;
            fp->greada = &GIO_int_defer;
            fp->greadb = &GIO_greadb_raw;
            fp->greadc = &GIO_greadc_raw;
            fp->gunread = &gunread;
            fp->gflushr = &GIO_gflushr_raw;
            break;
      #endif /* GIO_RAWIO_IS_SUPPORTED */
      case GIO_IO_COPY_READ:
      default:
         fp->state |= GIO_STATE_COPYR;

         fp->mod_r = fp->blksize;

         block_size = size ? fp->mod_r *
               ((size + fp->mod_r - 1) / fp->mod_r):
            fp->mod_r;

         if(mode & _IONON_CACHE) {
            /* development only, i.e. NOT production usage */
            fp->cache_r = fp->min_r = fp->max_r = 0;
            fp->big_r = INT_MAX;
         } else if(mode & _IONON_TUNED) {
            /* development only, i.e. NOT production usage */
            fp->cache_r = fp->min_r = fp->max_r =
               (size > 1) ? size : fp->blksize;
            fp->big_r = INT_MAX;
         } else if(fp->genus == GIO_MEDIA_GENUS_TAPE) {
            fp->cache_r = fp->min_r = fp->max_r = block_size;
            fp->big_r = INT_MAX;
         } else if(size) {
            fp->min_r = 0;
            fp->max_r = fp->cache_r = block_size;
            fp->big_r = block_size;
         } else {
            /*********************************************************/
            /* Results of trytuneup test runs and read optimization. */
            /*********************************************************/
            #ifdef HOST_Sun
               fp->min_r   = 0;
               fp->max_r   =  64 * GIO_KB; /* 15 Jul 1998 @ 8k ptr */
               fp->big_r   =  64 * GIO_KB; /* 15 Jul 1998 @ 8k ptr */
               fp->cache_r = fp->big_r;
            #elif HOST_SGI
               fp->min_r   = 0;
               fp->max_r   = 1 * block_size;
               fp->big_r   = 1 * block_size;
               fp->cache_r = fp->big_r;
            #else
               fp->min_r   = 0;
               fp->max_r   = 4 * block_size;
               fp->big_r   = 1 * block_size;
               fp->cache_r = fp->big_r;
            #endif
         }
         fp->greadm = &GIO_int_defer;
         fp->greada = &GIO_int_defer;
         fp->greadb = &GIO_greadb_buf;
         fp->greadc = &GIO_greadc_buf;
         fp->gunread = &gunread;
         fp->gflushr = &GIO_gflushr_buf;
         break;
      }

      /*************************************/
      /* Initialize WRITE mode parameters. */
      /*************************************/

      switch(mode_write) {
      #ifdef GIO_MMAP_IS_SUPPORTED
         case GIO_IO_MMAP_WRITE:
            fp->state |= GIO_STATE_MAPW;

            fp->mod_w = gio.page_size;

            page_size = size ? fp->mod_w *
                  ((size + fp->mod_w - 1) / fp->mod_w):
               fp->mod_w;

            if(mode & _IONON_CACHE) {
               /* development only, i.e. NOT production usage */
               fp->cache_w = fp->min_w = fp->max_w = page_size;
               fp->big_w = INT_MAX;
            } else if(mode & _IONON_TUNED) {
               /* development only, i.e. NOT production usage */
               fp->cache_w = fp->min_w = fp->max_w = page_size;
               fp->big_w = INT_MAX;
            } else if(size) {
               fp->min_w = fp->mod_w;
               fp->max_w = fp->cache_w = page_size;
               fp->big_w = page_size;
            } else {
               #ifdef HOST_Sun
                  fp->min_w   = page_size;
                  fp->max_w   = fp->max_r;
                  fp->big_w   = fp->big_r;
                  fp->cache_w = fp->big_w;
               #elif HOST_SGI
                  fp->min_w   = page_size;
                  fp->max_w   = 4 * page_size;
                  fp->big_w   = 2 * page_size;
                  fp->cache_w = fp->big_w;
               #else
                  fp->min_w   = page_size;
                  fp->max_w   = 8 * page_size;
                  fp->big_w   = 2 * page_size;
                  fp->cache_w = fp->big_w;
               #endif
            }
            fp->gwritem = &GIO_int_defer;
            fp->gwritea = &GIO_int_defer;
            fp->gwriteb = &GIO_int_defer;
            fp->gwritec = &GIO_int_defer;
            fp->gflushw = &GIO_gflushw_map;
            break;
      #endif /* GIO_MMAP_IS_SUPPORTED */
      #ifdef GIO_RAWIO_IS_SUPPORTED
         case GIO_IO_RAW_WRITE:
            fp->state |= GIO_STATE_RAWW;

            if(fp->align_p < fp->blkalign) fp->align_p = fp->blkalign;

            fp->mod_w = fp->minsize;

            block_size = size ? fp->mod_w *
                  ((size + fp->mod_w - 1) / fp->mod_w):
               fp->mod_w;

            if(mode & _IONON_CACHE) {
               /* development only, i.e. NOT production usage */
               fp->cache_w = fp->min_w = fp->max_w = block_size; /* ? = 0 */
               fp->big_w = INT_MAX;
            } else if(mode & _IONON_TUNED) {
               /* development only, i.e. NOT production usage */
               fp->cache_w = fp->min_w = fp->max_w = block_size;
               fp->big_w = INT_MAX;
            } else if(size) {
               fp->min_w = fp->mod_w;
               fp->max_w = fp->cache_w = block_size;
               fp->big_w = block_size;
            } else {
               #ifdef HOST_SGI
                  fp->min_w   = block_size;
                  fp->max_w   = 64 * block_size;
                  fp->big_w   = 8 * block_size;
                  fp->cache_w = fp->big_w;
               #else
                  fp->min_w   = block_size;
                  fp->max_w   = 16 * block_size;
                  fp->big_w   = 2 * block_size;
                  fp->cache_w = fp->big_w;
               #endif
            }
            if(fp->min_w < fp->minsize) fp->min_w = fp->minsize;
            if(fp->max_w > fp->maxsize) fp->max_w = fp->maxsize;
            if(fp->big_w < fp->minsize) fp->big_w = fp->minsize;
            if(fp->big_w > fp->maxsize) fp->big_w = fp->maxsize;

            fp->gwritem = &GIO_int_defer;
            fp->gwritea = &GIO_int_defer;
            fp->gwriteb = &GIO_int_defer;
            fp->gwritec = &GIO_int_defer;
            fp->gflushw = &GIO_gflushw_raw;
            break;
      #endif /* GIO_RAWIO_IS_SUPPORTED */
      case GIO_IO_COPY_WRITE:
      default:
         fp->state |= GIO_STATE_COPYW;

         fp->mod_w = fp->blksize;

         block_size = size ? fp->mod_w *
               ((size + fp->mod_w - 1) / fp->mod_w):
            fp->mod_w;

         if(mode & _IONON_CACHE) {
            /* development only, i.e. NOT production usage */
            fp->cache_w = fp->min_w = fp->max_w = 0;
            fp->big_w = INT_MAX;
         } else if(mode & _IONON_TUNED) {
            /* development only, i.e. NOT production usage */
            fp->cache_w = fp->min_w = fp->max_w =
               (size > 1) ? size : fp->blksize;
            fp->big_w = INT_MAX;
         } else if(fp->genus == GIO_MEDIA_GENUS_TAPE) {
            fp->cache_w = fp->min_w = fp->max_w = block_size;
            fp->big_w = INT_MAX;
         } else if(size) {
            fp->min_w = 0;
            fp->max_w = fp->cache_w = block_size;
            fp->big_w = block_size;
         } else {
            /*********************************************************/
            /* Nominal write size is the same as optimum read size,  */
            /* otherwise read back efficiency suffers, i.e.          */
            /* do NOT optimize write block size independently.       */
            /*********************************************************/
            #ifdef HOST_Sun
               fp->min_w   = 0;
               fp->max_w   = fp->max_r;
               fp->big_w   = fp->big_r;
               fp->cache_w = fp->big_w;
            #elif HOST_SGI
               fp->min_w   = 0;
               fp->max_w   = 1 * block_size;
               fp->big_w   = 1 * block_size;
               fp->cache_w = fp->big_w;
            #else
               fp->min_w   = 0;
               fp->max_w   = 8 * block_size;
               fp->big_w   = 2 * block_size;
               fp->cache_w = fp->big_w;
            #endif
         }
         fp->gwritem = &GIO_int_defer;
         fp->gwritea = &GIO_int_defer;
         fp->gwriteb = &GIO_gwriteb_buf;
         fp->gwritec = &GIO_gwritec_buf;
         fp->gflushw = &GIO_gflushw_buf;
         break;
      }
      break;
   case _IONBF: /* non buffered */
      if (gio.verbose) {
         fprintf(stderr, "WARNING gsetvbuf: _IONBF, using unbuffered io for %s\n",
                 fp->cfd.name);
      }

      /*****************/
      /* NON-BUFFERED. */
      /*****************/

      fp->state |= (GIO_STATE)(GIO_STATE_COPYR | GIO_STATE_COPYW);

      /**********************************************/
      /* Initialize READ and WRITE mode parameters. */
      /**********************************************/
      fp->mod_r = fp->blksize;
      fp->min_r = fp->max_r = fp->big_r = fp->cache_r = 0;

      fp->mod_w = fp->blksize;
      fp->min_w = fp->max_w = fp->big_w = fp->cache_w = 0;

      fp->greadm = &GIO_int_defer;
      fp->greada = &GIO_int_defer;
      fp->greadb = &GIO_greadb_non;
      fp->greadc = &GIO_greadc_non;
      fp->gunread = &gunread;
      fp->gflushr = &GIO_gflushr_non;
      fp->gwritem = &GIO_int_defer;
      fp->gwritea = &GIO_int_defer;
      fp->gwriteb = &GIO_gwriteb_non;
      fp->gwritec = &GIO_gwritec_non;
      fp->gflushw = &GIO_gflushw_non;
      break;
   }

   fp->mod_p = (fp->mod_r > fp->mod_w) ? fp->mod_r : fp->mod_w;

   /* Allow environment, GIO_BLKSIZE, to set the cache sizes */
   if (fp->genus == GIO_MEDIA_GENUS_DISK) { 
      if (gio.blk_size > 0) {
         fprintf(stderr, "WARNING gsetvbuf: setting cache size to %d\nfd=%d  name=%s\n",
              (int)gio.blk_size, fp->fd, fp->cfd.name);
         fp->cache_r = fp->min_r = fp->max_r = gio.blk_size;
         fp->cache_w = fp->min_w = fp->max_w = gio.blk_size;
      }
   } 

/***
fprintf(stderr, "   WARNING gsetvbuf: size= %li, max_r= %li, big_r= %li, max_w= %li, big_w= %li\n",
   (long)size,
   (long)fp->max_r, (long)fp->big_r, (long)fp->max_w, (long)fp->big_w);
***/

   return 0;
}
