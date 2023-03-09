#include <stdlib.h>
#include "gio.h"

int main(int argc, const char** argv)
{
   FILE *fp;
   int *buf;
   int bytes, n, size = 5 * 1024;

   printf("hello world\n");

   fp = fopen("/dev/nrst1", "w+");

   bytes = size * sizeof(int);

   buf = (int*)malloc(bytes);

   for(n = 0; n < size; n++) buf[n] = n;

   for(n = 0; n < 7; n++) fwrite(buf, 1, bytes, fp);

   exit(0);
}
