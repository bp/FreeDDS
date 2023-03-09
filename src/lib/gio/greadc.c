static char rcsid[]="$Id: greadc.c 13 2009-01-06 16:56:43Z user $";
/*
READ (buffer)

*/

#include "gio_sys.h"

int greadc(             /* actual byte count (error < 0) */
   GFILE *fp,           /* i/o stream */
   void *buf,           /* read buffer pointer */
   size_t size)         /* requested byte count */
{
   if(GIO_cfd_check(fp) < 0) return -1;
    
   if(fp->count_r > size && fp->big_r > size) {
      fp->count_r -= size;
      memcpy(buf, fp->buf, size);
      fp->buf += size;
      return size;
   }
   return fp->greadc(fp, buf, (GIO_BIG)size);
}
