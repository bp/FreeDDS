static char rcsid[]="$Id: operator.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, operator message, run time assistance.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#include <time.h>

int ddsOperator(
   const char *fmt,
   ...)
{
   int n;
   char msg[DDS_MSG_MAX+1];
   va_list args;
   time_t calendar_time;
   char msg_date[64];
 
   va_start(args, fmt);

   calendar_time = time(NULL);
   strcpy(msg_date, ctime(&calendar_time));
   msg_date[strlen(msg_date) - 1] = '\0';  /* remove trailing '\n' */

   /* message preamble is "DDS operator: PID <pid>:<alarm> <date>" */
   sprintf(msg, "DDS: OPERATOR: PID %i:%c %s\n\t",
      dds_cmd_pid, DDS_ASCII_ALERT, msg_date);

   /* Message body supplied by caller. */
   /* Intermediate lines terminated by "\n\t", last line by "\n". */
   n = strlen(msg);
   vsprintf(msg + n, fmt, args);
   va_end(args);

   if(dds_state & DDS_OPERATOR) ddsPrintStr(msg);
   if(dds_state & DDS_OPERATOR_LOG) ddsPrintLog("operator_msg", msg);

   return 0;
}
