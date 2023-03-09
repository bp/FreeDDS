static char rcsid[]="$Id: gclearerr.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, CLEAR ERRor and eof status

*/

#include "gio_sys.h"

void (gclearerr)(
   GFILE *fp)           /* file pointer */
{
   fp->state &= (GIO_STATE)~GIO_STATE_EOF;
   fp->error = 0;
   return;
}
