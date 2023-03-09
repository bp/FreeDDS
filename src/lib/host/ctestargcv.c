static char rcsid[]="$Id: ctestargcv.c 50 2009-02-10 21:29:01Z user $";
/*
RCS-t=HOST, main, test argcv linkage

*/

#include <stdio.h>

int setargcv(int argc, char **argv);

/*! TEST Program: test argcv linkage */
int main(
   int argc,
   char* argv[])
{
   setargcv(argc, argv);
   printf("hello world\n");
}
