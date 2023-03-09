static char rcsid[]="$Id: gflushr_non.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, FLUSH READ (non-buffered)

*/

#include "gio_sys.h"

int GIO_gflushr_non(   /* return 0 (error == EOF) */
   GFILE *fp)           /* GIO stream */
{
   /* ANSI standard implies that flush on read is undefined. */
   /* This is my unofficial definition of flush on read stream */
   int rc = 0;

   if(fp->state & GIO_STATE_UNREAD) {
      /* flush the unread cache */
      fp->state &= (GIO_STATE) ~ GIO_STATE_UNREAD;
   }

   fp->state &= (GIO_STATE) ~ GIO_STATE_KEOF;
   fp->start = fp->buf = fp->base_p;
   fp->waste = 0;

   return rc;
}
