static char rcsid[]="$Id: testptr2.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test pointer schemes

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <stdio.h>

void testptr2(
   char *msg,
   char *varg,
   char *carg,
   int *iarg,
   float *farg,
   long larg)
{
   printf("%24s, varg= %p, carg= %p, iarg= %p, farg= %p, larg= %li\n",
      msg, varg, carg, iarg, farg, larg);
   return;
}
