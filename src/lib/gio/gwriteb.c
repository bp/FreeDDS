static char rcsid[]="$Id: gwriteb.c 13 2009-01-06 16:56:43Z user $";
/*
WRITE Biased pointer

*/

#include "gio_sys.h"

int gwriteb(            /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   size_t size,         /* requested byte count */
   GIO_ALIGN align,        /* buffer alignment */
   size_t bias)         /* alignment bias */
{
   if(GIO_cfd_check(fp) < 0) return -1;

   if(fp->count_w > size && GIO_PTR_BIASED(fp->buf, align, bias)) {
      fp->count_w -= size;
      *buf = fp->buf;
      fp->buf += size;
      return size;
   } 
   return fp->gwriteb(fp, buf, (GIO_BIG)size, align, (size_t)bias);
}
