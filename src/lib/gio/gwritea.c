static char rcsid[]="$Id: gwritea.c 13 2009-01-06 16:56:43Z user $";
/*
WRITE Aligned pointer

*/

#include "gio_sys.h"

int gwritea(            /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   void **buf,          /* write buffer pointer to pointer */
   size_t size,         /* requested byte count */
   GIO_ALIGN align)        /* buffer alignment */
{
   if(GIO_cfd_check(fp) < 0) return -1;

   if(fp->count_w > size && GIO_PTR_ALIGNED(fp->buf, align)) {
      fp->count_w -= size;
      *buf = fp->buf;
      fp->buf += size;
      return size;
   }
   return fp->gwriteb(fp, buf, (GIO_BIG)size, align, (size_t)0);
}
