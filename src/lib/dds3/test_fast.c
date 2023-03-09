static char rcsid[]="$Id: test_fast.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test parameter dictionary simplest functionality.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int main(
   int argc,
   char **argv)
{
   char buf[1000];
   const char *fast;

   setargcv(argc, argv);
 
   cdds_init();

   while(fprintf(stdout, "test_fast: ? filename="), buf[0] = '\0',
      1 == fscanf(stdin, "%s", buf) || buf[0]) {
      fast = ddsPathFast(buf);
      fprintf(stdout, "fast= %s\n", fast);
   }

   return 0;
}
