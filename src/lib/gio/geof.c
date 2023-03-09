static char rcsid[]="$Id: geof.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, End Of File status

*/

#include "gio_sys.h"

int (geof)(             /* non-zero = EOF is set */
   GFILE *fp)           /* file pointer */
{
   return (fp->state & GIO_STATE_EOF);
}
