static char rcsid[]="$Id: defer.c 13 2009-01-06 16:56:43Z user $";
/*
deferred functionality

*/

#include "gio_sys.h"

int GIO_int_defer()
{
   fprintf(stderr,"gio: sorry, (int) function deferred\n");

   assert(! "gio: sorry, (int) function deferred");
   abort();
   exit(1);
   return 0;
}

HOST_INT8_T xGIO_offset_defer()
{
   fprintf(stderr,"gio: sorry, (HOST_INT8_T) function deferred\n");

   assert(! "sorry, (HOST_INT8_T) function deferred");
   abort();
   exit(1);
   return HOST_LONG_TO_INT8(0);
}

GFILE * xGIO_fileb_defer()
{
   fprintf(stderr,"gio: sorry, (GFILE *) function deferred\n");

   assert(! "sorry, (GFILE *) function deferred");
   abort();
   exit(1);
   return NULL;
}
