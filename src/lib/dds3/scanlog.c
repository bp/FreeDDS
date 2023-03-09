static char rcsid[]="$Id: scanlog.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, scan "log:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1997                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 11 March 1997          ***/
/**********************************************/

/**********************************************/
/*  NOTE:!!!                                  */
/*  This is only used by readlog.c            */
/*  log_scan_save must be reinitialized by    */
/*  a dummy call to ddsScanLog with no alias. */
/**********************************************/

#include "dds_sys.h"

int ddsScanLog(
   const char *alias,
   const char **value)
{
   int status = DDS_OK;
   static char log_scan_save='\0';
   static size_t log_scan_loc=0;
   size_t save_size, scan_value_bias;
   const DDS_DEFN *defn;
   DDS_TOKEN scan_token;    
   DDS_ALIAS *list;
   const DDS_DEFN *scan_newer;

   if(!dds_log_dict) {
      ddsDebug("'log:' dictionary not opened for internal scanning!\n");
      return DDS_BAD;
   }

   /* restore saved character from any previous log scan */
   if (log_scan_save) {
      dds_log_dict->buf.ptr[log_scan_loc] = log_scan_save;
      log_scan_save = '\0';
   }

   if(alias) while(isspace(*alias)) alias++;

   scan_token.str.max = 0;
   scan_token.str.ptr = (char*)0;
   scan_token.next = (const char*)0;

   /* append new definition name */
   if(alias && *alias) {
      /* initialize alias list (definition names) */
      list = ddsAliasList(NULL, alias);

      /* search for a defn, alias exist */
      defn = ddsDefnFind(&save_size, &scan_value_bias, &list, 
            dds_log_dict);
      if(defn) {
         /* DEFINITION FOUND */
         scan_newer = (DDS_DEFN*)defn->newer;
         log_scan_loc  = scan_newer->start;
         log_scan_save = dds_log_dict->buf.ptr[scan_newer->start];
         dds_log_dict->buf.ptr[scan_newer->start] = '\0';
         scan_token.next = dds_log_dict->buf.ptr + scan_value_bias;
      }
      ddsAliasDone(list);
   }
 
   status = ddsToken(&scan_token, NULL);
   if(status != EOF) *value = scan_token.str.ptr;

   return status;
}
