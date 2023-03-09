static char rcsid[]="$Id: gerror.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, return ERROR status

*/

#include "gio_sys.h"

int (gerror)(           /* non-zero = Error is set */
   GFILE *fp)           /* file pointer */
{
   return fp->error;
}
