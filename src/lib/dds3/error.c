static char rcsid[]="$Id: error.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, error message and optional abort.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsError(
   const char *fmt,
   ...)
{
   DDS_BOOLEAN print_msg, print_log, state_msg;
   int n;
   char msg[DDS_MSG_MAX+1];
   va_list args;
 
   va_start(args, fmt);

   if(dds_state & (DDS_VERBOSE | DDS_ERROR_ABORT)) {
      state_msg = DDS_TRUE;
   } else {
      state_msg = DDS_FALSE;
   }
 
   if(dds_state & (DDS_VERBOSE | DDS_ERROR_ABORT | DDS_ERROR_PRINT)) {
      print_msg = DDS_TRUE;
   } else {
      print_msg = DDS_FALSE;
   }
 
   if(dds_state & DDS_ERROR_LOG) {
      print_log = DDS_TRUE;
   } else {
      print_log = DDS_FALSE;
   }

   dds_error_count++;
 
   strcpy(msg, "DDS ERROR: ");

   /* Message body supplied by caller. */
   /* Intermediate lines terminated by "\n\t", last line by "\n". */
   n = strlen(msg);
   vsprintf(msg + n, fmt, args);
   va_end(args);
   if(print_msg) ddsPrintStr(msg);
   if(print_log) ddsPrintLog("error_msg", msg);
   ddsStrCpy(&dds_error_msg, msg);

   if(state_msg) {
      sprintf(msg, "\tAPI stack");
      if(print_msg) ddsPrintStr(msg);
      if(print_log) ddsPrintLog("", msg);
      ddsStrCat(&dds_error_msg, msg);
      for(n = 0; n < dds_api_next; n++) {
         sprintf(msg, " => %s", dds_api_call[n]);
         if(print_msg) ddsPrintStr(msg);
         if(print_log) ddsPrintLog("", msg);
         ddsStrCat(&dds_error_msg, msg);
      }
      if(print_msg) ddsPrintStr("\n");
      if(print_log) ddsPrintLog("", "\n");
      ddsStrCat(&dds_error_msg, "\n");

      sprintf(msg, "\terror count= %i,  warning count= %i\n",
         dds_error_count, dds_warn_count);
      if(print_msg) ddsPrintStr(msg);
      if(print_log) ddsPrintLog("", msg);
      ddsStrCat(&dds_error_msg, msg);
   }

   if(state_msg && dds_scan_dict) {
      sprintf(msg, "\tSCAN DICT= %s,  DEFN= %s\n",
         dds_scan_dict->name, ddsHint(dds_scan_name));
      if(print_msg) ddsPrintStr(msg);
      if(print_log) ddsPrintLog("", msg);
      ddsStrCat(&dds_error_msg, msg);
   }

   if(state_msg && dds_print_first) {
      sprintf(msg, "\tPRINT DICT= %s,  DEFN= %s\n",
         dds_print_first->name, ddsHint(dds_print_first->print_name.ptr));
      if(print_msg) ddsPrintStr(msg);
      if(print_log) ddsPrintLog("", msg);
      ddsStrCat(&dds_error_msg, msg);
   }

   if(dds_state & DDS_ERROR_ABORT) {
      ddsPrintStr("\tERROR FUNCTION CALLING ABORT...  :-(\n");
      ddsStrCat(&dds_error_msg, msg);
      ddsAbort();
   }

   return errno ? errno : (dds_error ? dds_error : EINVAL) ;
}
