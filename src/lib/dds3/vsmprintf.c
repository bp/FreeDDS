static char rcsid[]="$Id: vsmprintf.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, variable length formatted message, C interface.

*/

/***********************************/
/***   Copyright (C) 2004        ***/
/***   AP AMERICA, Houston, TX   ***/
/***   Jerry Ehlers, March 2004  ***/
/***********************************/

#include <limits.h>
#include "dds_sys.h"

#define DDS_MSG_MAX_MAX 1000 * DDS_MSG_MAX

static size_t mlen=0;
static char *msg;

char* ddsVsmprintf(const char *fmt, va_list args)
{
   size_t slen;

   if (!mlen) {
      mlen = DDS_MSG_MAX;
      msg = (char*)malloc(mlen+1);
   }

   slen = vsnprintf(msg, mlen, fmt, args);
   while(slen >= mlen && mlen < DDS_MSG_MAX_MAX) {
      mlen = 2*mlen;
      msg = (char*)realloc(msg, mlen+1);
      slen = vsnprintf(msg, mlen, fmt, args);
   }

   if (slen >= mlen) {
      ddsError("ddsVsmprintf: buffer maxed out at %d\n%s\n",
               slen, msg);
   }

   return msg;
}
