static char rcsid[]="$Id: gsetbuf.c 13 2009-01-06 16:56:43Z user $";
/*
SETBUF

*/

#include "gio_sys.h"

void gsetbuf(
   GFILE *fp,           /* GIO stream */
   void *buf)           /* buffer pointer */
{
   if(buf) {
      /* standard violation, in that size should be BUFSIZ */
      /* but this would be *stupid* for GIO */
      gsetvbuf(fp, buf, _IOFBF, 0);
   } else {
      gsetvbuf(fp, buf, _IONBF, 0);
   }

   return;
}
