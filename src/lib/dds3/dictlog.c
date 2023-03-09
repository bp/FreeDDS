static char rcsid[]="$Id: dictlog.c 16 2009-01-06 21:52:47Z user $"; 
/*
RCS-t=DDS, open and initialize "log:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1997                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 6 March 1997           ***/
/**********************************************/

#include "dds_sys.h"

#define PRINT(a,b) \
   sprintf(msg, "%s", (dds_state & (b)) ? "true" : "false" ); \
   ddsPrintLog( (a), msg );

DDS_DICT * ddsDictLog(
   const char *mode,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state)
{
   char msg[DDS_MSG_MAX+1];

   if(dict_state & DDS_DICT_READ_ONLY) {
      ddsError("dict: errno= %i, %s\n\topening 'log:' dictionary, mode= %s\n",
         EACCES, strerror(EACCES), mode);
      return NULL;
   }

   /* reset abort/print flags */

   dds_state = (DDS_STATE)(dds_state & ~ (DDS_ERROR_LOG 
      | DDS_WARN_ABORT | DDS_WARN_LOG | DDS_DEBUG_ABORT 
      | DDS_DEBUG_LOG | DDS_OPERATOR_LOG | DDS_VERBOSE_LOG));

   dds_state = (DDS_STATE)(dds_state | DDS_ERROR_ABORT 
      | DDS_ERROR_PRINT | DDS_WARN_PRINT | DDS_DEBUG_PRINT 
      | DDS_OPERATOR);
 
   dds_warn_count = dds_error_count = dds_debug_count = 0;

   /* open new tmp dictionary */

   dds_log_dict = ddsDictTmp("log:", mode, dict_state);

   /* initialize value in log: file */

   PRINT("error_abort",    DDS_ERROR_ABORT);
   PRINT("error_print",    DDS_ERROR_PRINT);
   PRINT("error_log",      DDS_ERROR_LOG);
   PRINT("warn_abort",     DDS_WARN_ABORT);
   PRINT("warn_print",     DDS_WARN_PRINT);
   PRINT("warn_log",       DDS_WARN_LOG);
   PRINT("debug_abort",    DDS_DEBUG_ABORT);
   PRINT("debug_print",    DDS_DEBUG_PRINT);
   PRINT("debug_log",      DDS_DEBUG_LOG);
   PRINT("operator_print", DDS_OPERATOR);
   PRINT("operator_log",   DDS_OPERATOR_LOG);
   PRINT("verbose_print",  DDS_VERBOSE);
   PRINT("verbose_log",    DDS_VERBOSE_LOG);

   return dds_log_dict;
}
