static char rcsid[]="$Id: makedict.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, compile dictionary (stdin:) into "C" string

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <stdio.h>
#include <chost.h>

int main(
   int argc,
   char **argv)
{
   int c;

   fputs("\"", stdout);

   for(c = getc(stdin); c > 0; c = getc(stdin)) {
      switch(c) {
      case '\n':
         #ifdef HOST_Linux
            /* special case, concantenate adjacent strings */
            fputs("\\n\"\n\"", stdout);
         #else
            /* default case, single string, escape newlines */
            fputs("\\n\\\n", stdout);
         #endif
         break;
      case '"':
      case '\\':
         putc('\\', stdout);
         putc(c, stdout);
         break;
      default:
         putc(c, stdout);
         break;
      }
   }
   fputs("\";\n", stdout);

   return 0;
}
