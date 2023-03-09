static char rcsid[]="$Id: verbose.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, vervbose message, optionally printed.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsVerbose(
   const char *fmt,
   ...)
{
   int n;
   char msg[DDS_MSG_MAX+1];
   va_list args;

   if(! (dds_state & (DDS_VERBOSE | DDS_VERBOSE_LOG))) return 0;
 
   va_start(args, fmt);
 
   strcpy(msg, "DDS VERBOSE: ");

   /* Message body supplied by caller. */
   /* Intermediate lines terminated by "\n\t", last line by "\n". */
   n = strlen(msg);
   vsprintf(msg + n, fmt, args);
   va_end(args);
   if( dds_state & DDS_VERBOSE) ddsPrintStr(msg);
   if( dds_state & DDS_VERBOSE_LOG) ddsPrintLog("verbose_msg", msg);

   return 0;
}
