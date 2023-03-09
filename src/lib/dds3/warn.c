static char rcsid[]="$Id: warn.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, warning message and optional abort.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsWarn(
   const char *fmt,
   ...)
{
   DDS_BOOLEAN print_msg, print_log, verbose_msg;
   int n;
   char msg[DDS_MSG_MAX+1];
   va_list args;
 
   va_start(args, fmt);

   if(dds_state & (DDS_VERBOSE | DDS_WARN_ABORT)) {
      verbose_msg = DDS_TRUE;
   } else {
      verbose_msg = DDS_FALSE;
   }
 
   if(dds_state & (DDS_VERBOSE | DDS_WARN_ABORT | DDS_WARN_PRINT)) {
      print_msg = DDS_TRUE;
   } else {
      print_msg = DDS_FALSE;
   }
 
   if(dds_state & DDS_WARN_LOG) {
      print_log = DDS_TRUE;
   } else {
      print_log = DDS_FALSE;
   }

   dds_warn_count++;
 
   strcpy(msg, "DDS WARNING: ");

   /* Message body supplied by caller. */
   /* Intermediate lines terminated by "\n\t", last line by "\n". */
   n = strlen(msg);
   vsprintf(msg + n, fmt, args);
   va_end(args);
   if(print_msg) ddsPrintStr(msg);
   if(print_log) ddsPrintLog("warn_msg", msg);
   ddsStrCpy(&dds_warn_msg, msg);

   if(verbose_msg) {
      sprintf(msg, "\tAPI stack");
      if(print_msg) ddsPrintStr(msg);
      if(print_log) ddsPrintLog("", msg);
      ddsStrCat(&dds_warn_msg, msg);
      for(n = 0; n < dds_api_next; n++) {
         sprintf(msg, " => %s", dds_api_call[n]);
         if(print_msg) ddsPrintStr(msg);
         if(print_log) ddsPrintLog("", msg);
         ddsStrCat(&dds_warn_msg, msg);
      }
      if(print_msg) ddsPrintStr("\n");
      if(print_log) ddsPrintLog("", "\n");
      ddsStrCat(&dds_warn_msg, "\n");

      sprintf(msg, "\terror count= %i,  warning count= %i\n",
         dds_error_count, dds_warn_count);
      if(print_msg) ddsPrintStr(msg);
      if(print_log) ddsPrintLog("", msg);
      ddsStrCat(&dds_warn_msg, msg);
   }

   if(verbose_msg && dds_scan_dict) {
      sprintf(msg, "\tSCAN DICT= %s,  DEFN= %s\n",
         dds_scan_dict->name, ddsHint(dds_scan_name));
      if(print_msg) ddsPrintStr(msg);
      if(print_log) ddsPrintLog("", msg);
      ddsStrCat(&dds_warn_msg, msg);
   }

   if(verbose_msg && dds_print_first) {
      sprintf(msg, "\tPRINT DICT= %s,  DEFN= %s\n",
         dds_print_first->name, ddsHint(dds_print_first->print_name.ptr));
      if(print_msg) ddsPrintStr(msg);
      if(print_log) ddsPrintLog("", msg);
      ddsStrCat(&dds_warn_msg, msg);
   }

   if(dds_state & DDS_WARN_ABORT) {
      ddsPrintStr("\tWARN FUNCTION CALLING ABORT...  :-(\n");
      ddsStrCat(&dds_warn_msg, msg);
      ddsAbort();
   }

   return errno ? errno : (dds_error ? dds_error : EINVAL) ;
}
