static char rcsid[]="$Id: gwritec.c 13 2009-01-06 16:56:43Z user $";
/*
WRITE copy buffer

*/

#include "gio_sys.h"

int gwritec(            /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   const void *buf,     /* write buffer pointer */
   size_t size)         /* requested byte count */
{
   if(GIO_cfd_check(fp) < 0) return -1;

   if(fp->count_w > size && fp->big_w > size) {
      fp->count_w -= size;
      memcpy(fp->buf, buf, size);
      fp->buf += size;
      return size;
   }
   return fp->gwritec(fp, buf, (GIO_BIG)size);
}
