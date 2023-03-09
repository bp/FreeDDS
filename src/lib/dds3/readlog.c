static char rcsid[]="$Id: readlog.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, update flags from "log:" dictionary .

*/

/**********************************************/
/***   Copyright (C) 1997                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 11 March 1997          ***/
/**********************************************/

#include "dds_sys.h"


#define SCAN(a,b) \
   msg = (dds_state & (b)) ? "true" : "false"; \
   ddsScanLog( (a), &msg ); \
   if (strstr(msg, "true")) dds_state = (DDS_STATE)(dds_state | (b)); \
   else dds_state = (DDS_STATE)(dds_state & ~(b)); 

void ddsReadLog()
{
   const char *msg;

   /* reinitialize values from log: file */

   SCAN("error_abort",    DDS_ERROR_ABORT);
   SCAN("error_print",    DDS_ERROR_PRINT);
   SCAN("error_log",      DDS_ERROR_LOG);
   SCAN("warn_abort",     DDS_WARN_ABORT);
   SCAN("warn_print",     DDS_WARN_PRINT);
   SCAN("warn_log",       DDS_WARN_LOG);
   SCAN("debug_abort",    DDS_DEBUG_ABORT);
   SCAN("debug_print",    DDS_DEBUG_PRINT);
   SCAN("debug_log",      DDS_DEBUG_LOG);
   SCAN("operator_print", DDS_OPERATOR);
   SCAN("operator_log",   DDS_OPERATOR_LOG);
   SCAN("verbose_print",  DDS_VERBOSE);
   SCAN("verbose_log",    DDS_VERBOSE_LOG);

   /* dummy scan to reset log scan save character */
   ddsScanLog("", &msg ); \

   return;
}
