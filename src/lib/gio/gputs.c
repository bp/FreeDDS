static char rcsid[]="$Id: gputs.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, PUT String

*/

#include "gio_sys.h"

int (gputs)(            /* next character (error == EOF) */
   const char *str,     /* character string to write */
   GFILE *fp)           /* i/o stream */
{
   size_t n = strlen(str);

   if(n == gwritec(fp, str, n)) return 0;

   return EOF; /* error */
}
