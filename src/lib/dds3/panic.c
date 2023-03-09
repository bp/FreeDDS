static char rcsid[]="$Id: panic.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, panic message and forced abort.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_dump.h"

int ddsPanic(
   const char *fmt,
   ...)
{
   int n;
   char msg[DDS_MSG_MAX+1];
   va_list args;
 
   dds_state |= DDS_CLEANUP; /* inhibit cleanup */

   if(dds_debug_api & DDS_DBG_PANIC) {
      ddsPrintStr("*******************************\n"
                  "***  DDS PANIC BINARY DUMP  ***\n"
                  "*******************************\n");
      ddsDumpBinary();
   }
 
   va_start(args, fmt);

   strcpy(msg, "DDS PANIC: ");

   /* Message body supplied by caller. */
   /* Intermediate lines terminated by "\n\t", last line by "\n". */
   n = strlen(msg);
   vsprintf(msg + n, fmt, args);
   va_end(args);
   ddsPrintStr(msg);

   sprintf(msg, "\tAPI stack");
   ddsPrintStr(msg);
   for(n = 0; n < dds_api_next; n++) {
      sprintf(msg, " => %s", dds_api_call[n]);
      ddsPrintStr(msg);
   }
   ddsPrintStr("\n");

   sprintf(msg, "\tdebug count= %i,  error count= %i,  warning count= %i\n",
      dds_debug_count, dds_error_count, dds_warn_count);
   ddsPrintStr(msg);

   if(dds_scan_dict) {
      sprintf(msg, "\tSCAN DICT= %s,  DEFN= %s\n",
         dds_scan_dict->name, ddsHint(dds_scan_name));
      ddsPrintStr(msg);
   }

   if(dds_print_first) {
      sprintf(msg, "\tPRINT DICT= %s,  DEFN= %s\n",
         dds_print_first->name, ddsHint(dds_print_first->print_name.ptr));
      ddsPrintStr(msg);
   }

   ddsPrintStr("\tPANIC FUNCTION CALLING ABORT... 8*(\n");

   ddsAbort();
   return 1; /* never */
}
