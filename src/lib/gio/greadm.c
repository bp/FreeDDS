static char rcsid[]="$Id: greadm.c 13 2009-01-06 16:56:43Z user $";
/*
READ Malloc pointer

*/

#include "gio_sys.h"

int greadm(              /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void **buf,    /* read buffer pointer to pointer */
   size_t size)         /* requested byte count */
{
   if(GIO_cfd_check(fp) < 0) return -1;

   if(fp->count_r > size && GIO_PTR_ALIGNED(fp->buf, GIO_MALLOC)) {
      fp->count_r -= size;
      *buf = fp->buf;
      fp->buf += size;
      return size;
   }
   return fp->greadb(fp, buf, (GIO_BIG)size, GIO_MALLOC, (size_t)0);
}
