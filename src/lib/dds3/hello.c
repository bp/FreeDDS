static char rcsid[]="$Id: hello.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test compiler simplest functionality.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <stdio.h>

int main(
   const int argc,
   char **argv)
{
   typedef int [3] xxx;
   typedef struct YYY{xxx x; char y;} yyy;

   printf("size = %i  %i\n", (int)sizeof (xxx), (int)sizeof (yyy));
   return 0;
}
