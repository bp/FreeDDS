static char rcsid[]="$Id: ggetpos.c 13 2009-01-06 16:56:43Z user $";
/*
GET current POSition (offset)

*/

#include "gio_sys.h"

int GIO_getpos(         /* -1 = Error */
   GFILE *fp,           /* file pointer */
   gpos_t *offset)      /* current offset */
{
   if(fp->state & GIO_MASK_BUFFERED) {
      if(fp->state & GIO_STATE_READ) {
         if(fp->state & GIO_STATE_UNREAD) {
            /*** 8/2/96 rls, bug fix
               "For a binary stream, its file position indicator is 
               decremented by each successful call to the ungetc function;"
            ***/
            *offset = fp->offset + (HOST_INT6_T)(fp->buf_u - fp->start)
               - fp->count_r;
         } else {
            *offset = fp->offset + (HOST_INT6_T)(fp->buf - fp->start);
         }
      } else if(fp->state & GIO_STATE_WRITE) {
         *offset = fp->offset + (HOST_INT6_T)(fp->buf - fp->start);
      } else {
         *offset = fp->offset;
      }
   } else {
      if(fp->state & GIO_STATE_WRITE) {
         *offset = fp->offset + (HOST_INT6_T)(fp->buf - fp->start);
      } else {
         *offset = fp->offset;
      }
   }

   return 0;
}

int (ggetpos)(           /* -1 = Error */
   GFILE *fp,           /* file pointer */
   gpos_t *offset)      /* current offset */
{
   if(GIO_cfd_check(fp) < 0) return -1;

   return GIO_getpos(fp, offset);
}
