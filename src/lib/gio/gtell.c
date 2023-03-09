static char rcsid[]="$Id: gtell.c 13 2009-01-06 16:56:43Z user $";
/*
return long TELL current offset

*/

#include "gio_sys.h"

long (gtell)(           /* -1 = Error */
   GFILE *fp)           /* file pointer */
{
   gpos_t offset;

   if(! ggetpos(fp, &offset)) {
      if(offset == (HOST_LONG_T)offset)
         return offset;
   }
   return -1;
}
