#include <stdlib.h>
#include "gio.h"

int main(int argc, const char** argv)
{
   GFILE *fp;
   const int *buf;
   int bytes, rc, n, size = 5 * 1024;

   printf("hello world\n");

   fp = gopen("/dev/nrst1", "r+");

   bytes = size * sizeof(int);

   for(n = 0; n < 26; n++) {
      rc = greada(fp, (const void**)&buf, bytes, GIO_INT);
      fprintf(stderr, "n= %i, rc= %i\n", n, rc);
   }

   exit(0);
}
