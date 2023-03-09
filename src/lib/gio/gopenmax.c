static char rcsid[]="$Id: gopenmax.c 13 2009-01-06 16:56:43Z user $";
/*
CACHE FILE DESCRIPTOR API

*/

#include <stdarg.h>
#include "gio_sys.h"

int gopenmax(	  /* change gio.open_max, return new value (error<0) */
   GIO_OPEN_MAX_OPTION option,
   ...)
{
   int newvalue = -1;
   va_list ap;

   gio_init();

   va_start(ap, option);

   switch(option) {
   case GIO_OPEN_MAX_SET:
      newvalue = va_arg(ap,int);
      break;
   case GIO_OPEN_MAX_INCR:
      newvalue = gio.open_max + va_arg(ap,int);
      break;
   case GIO_OPEN_MAX_RESET:
      newvalue = gio.init_open_max;
      break;
   case GIO_OPEN_MAX_GET:
      newvalue = gio.open_max;
      break;
   }

   va_end(ap);

   if(newvalue >= 0) {
      gio.open_max = newvalue;
   }

   return newvalue;
}
