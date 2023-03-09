static char rcsid[]="$Id: ctest2.c 50 2009-02-10 21:29:01Z user $";
/*
RCS-t=HOST, main, test basic make functionality

*/

#include <stdio.h>

typedef struct
{
int x;
int y;
struct{short a;}z;
} XX[2];

/*! TEST Program: test basic make functionality */
int main(
   int argc,
   char **argv)
{
   char i1;
   short i2;
	int   i4;
	long  i8;
   long long i16;

   fprintf(stderr, "ctest2: sizeof XX   = %i\n", (int)sizeof(XX));
   fprintf(stderr, "ctest2: sizeof ptr  = %i\n", (int)sizeof(char*));
   fprintf(stderr, "ctest2: sizeof char = %i\n", (int)sizeof(char));
   fprintf(stderr, "ctest2: sizeof short= %i\n", (int)sizeof(short));
   fprintf(stderr, "ctest2: sizeof int  = %i\n", (int)sizeof(int));
   fprintf(stderr, "ctest2: sizeof long = %i\n", (int)sizeof(long));
   fprintf(stderr, "ctest2: sizeof llong= %i\n", (int)sizeof(long long));
   return 0;
}
