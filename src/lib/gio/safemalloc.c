static char rcsid[]="$Id: safemalloc.c 13 2009-01-06 16:56:43Z user $";
/*
RCS-t=GIO, safe malloc

*/

#include <stdio.h>
#include <stdlib.h>

void *safemalloc(
   size_t size)
{
   void *mem;

   mem = (void*)malloc(size);
   if(mem) return mem;

   fprintf(stderr, "tune: malloc error, %i bytes\n", size);
   exit(1);
   return NULL;
}
