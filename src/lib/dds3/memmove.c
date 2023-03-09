static char rcsid[]="$Id: memmove.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, copy char s2[n] to s1[n] safely.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <string.h>

void * memmove(
   void *s1,
   const void *s2,
   size_t n)
{
   char *sc1;
   const char *sc2;

   sc1 = s1;
   sc2 = s2;
   if(sc2 < sc1 && sc1 < sc2 + n)
      for(sc1 += n, sc2 += n; 0 < n; --n)
         *--sc1 = *--sc2;	/* copy backwards */
   else
      for(; 0 < n; --n)
         *sc1++ = *sc2++;	/* copy forwards */
   return (s1);
}
