#include <stdio.h>
#include "gio.h"

int main(int argc, const char** argv)
{
   GFILE *fp;
   const int *buf;
   int n, size = 5 * 1024;

   printf("hello world\n");

   fp = gopen("t1.dat", "r+");

   n = greadm(fp, (const void**)&buf, size * sizeof(int));

   exit(0);
}
