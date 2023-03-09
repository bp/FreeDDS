static char rcsid[]="$Id: tmpdict.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create a unique temporary dictionary name

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * ddsTmpDict(
   const char *clue)
{
   static int serial = 0;
   static char buf[PATHNAME_MAX+1];

#if defined(_WIN32) || defined(MINGW)
   sprintf(buf, "tmpDict%s%i-", clue, ++serial);
#else
   sprintf(buf, "tmpDict%s%i:", clue, ++serial);
#endif

   return buf;
}
