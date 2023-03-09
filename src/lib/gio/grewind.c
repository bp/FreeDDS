static char rcsid[]="$Id: grewind.c 13 2009-01-06 16:56:43Z user $";
/*
REWIND (offset == 0)

*/

#include "gio_sys.h"
 
void grewind(             /* new byte offset (error < 0) */
   GFILE *fp)           /* i/o stream */
{
   gpos_t n_huge;

   if(GIO_cfd_check(fp) < 0) return;

   /* initialize gstdin, gstdout, or gstderr, prior to access */
   if(! (fp->state & GIO_STATE_OPENED) &&
      ! GIO_gopen_init(fp)) return;

   n_huge = 0;
   gsetpos(fp, &n_huge);
   fp->error = 0;
   return;
}
