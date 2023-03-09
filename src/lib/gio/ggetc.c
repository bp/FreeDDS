static char rcsid[]="$Id: ggetc.c 13 2009-01-06 16:56:43Z user $";
/*
GET Character

*/

#include "gio_sys.h"

#undef ggetc

int (ggetc)(            /* next character (error == EOF) */
   GFILE *fp)           /* i/o stream */
{
   const void *buf;

   if(fp->count_r > 1) {
      fp->count_r--;
      return *(unsigned char*)(fp->buf++);
#ifdef AXIAN_REL_ALPHA_07
/* 7 May 1997 RLS, I don't think this is needed... */
/* Please enlighten me if it is. */
   } else if(fp->genus == GIO_MEDIA_GENUS_FIFO ||
             fp->genus == GIO_MEDIA_GENUS_SOCKET){
               char a_char;
               if(1 == fp->greadc(fp, (void *)&a_char, (GIO_BIG)1)) {
                 return (int)a_char;
               }
#endif
   } else if(1 == fp->greadb(fp, &buf, (GIO_BIG)1, GIO_CHAR, (size_t)0)) {
      return *(unsigned char*)buf;
   }

   return EOF; /* EOF or error */
}
