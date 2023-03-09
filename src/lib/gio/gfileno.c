static char rcsid[]="$Id: gfileno.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, return FILE descriptor Number

*/

#include "gio_sys.h"

int gfileno(            /* file descriptor */
   GFILE *fp)           /* file pointer */
{
   if(!fp->cfd.really_open) {
      if(GIO_open(fp) < 0) return -1;
   }

   return fp->fd;
}
