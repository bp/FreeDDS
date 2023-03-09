static char rcsid[]="$Id: gputc.c 13 2009-01-06 16:56:43Z user $";
/*
PUT Character

*/

#include "gio_sys.h"

#undef gputc

int (gputc)(            /* next character (error == EOF) */
   int c,               /* next character to write */
   GFILE *fp)           /* i/o stream */
{
   void *buf;

   if(fp->count_w > 1) {
      fp->count_w--;
      *(unsigned char*)(fp->buf++) = c;
      return c;
   } else if(1 == fp->gwriteb(fp, &buf, (GIO_BIG)1, GIO_CHAR, (size_t)0)) {
      *(unsigned char*)(buf) = c;
      return c;
   }

   return EOF; /* error */
}
