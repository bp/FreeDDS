static char rcsid[]="$Id: printlog.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, print to "log:" dictionary.

*/

/**********************************************/
/***   Copyright (C) 1997                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 11 March 1997          ***/
/**********************************************/

#include "dds_sys.h"

void ddsPrintLog(
   const char *alias,
   const char *value)
{
   int itest;
   const char *test;
   const char *start, *next;
   DDS_BOOLEAN quote1 = DDS_FALSE, quote2 = DDS_FALSE;
   size_t value_size = 0, token_size = 0;
   DDS_DEFN *newest_defn;

   if(!dds_log_dict) {
      ddsDebug("'log:' dictionary not opened for internal printing!\n");
      return;
   }

   if(alias) while(isspace(*alias)) alias++;

   /* append new definition name */
   if(alias && *alias) {
      for(test = alias;
         DEFN_SYNTAX(*test);
         test++) /* null */ ;
      newest_defn = (DDS_DEFN*)dds_log_dict->defn.newest;
      if(newest_defn->start &&
         ! isspace(dds_log_dict->buf.ptr[newest_defn->start - 1])) {
         /* insert '\n' (white space) name prefix */
         ddsDictLoad(dds_log_dict, &dds_log_dict->load,
            NULL, "\n", 1);
      }
      ddsDictLoad(dds_log_dict, &dds_log_dict->load,
         NULL, alias, (test - alias));
      ddsDictLoad(dds_log_dict, &dds_log_dict->load,
         NULL, "=", 1);
   }
 
   /* convert value string into definition token */
   ddsDictLoad(dds_log_dict, &dds_log_dict->load,
      NULL, " ", 1);
   start = next = value;
   for(itest = *next; itest; itest = *++next) {
      if(isspace(itest)) {
         quote1 = DDS_TRUE;
      } else if(! isgraph(itest) ||
         itest == '\\' ||
         itest == '"' ||
         itest == '=') {
         /* escape required for itest character */
         if(! quote2) {
            /* append opening quote */
            ddsDictLoad(dds_log_dict, &dds_log_dict->load,
               NULL, "\"", 1);
            quote2 = DDS_TRUE;
         }
         token_size = next - start;
         if(token_size)
            ddsDictLoad(dds_log_dict, &dds_log_dict->load,
               NULL, start, token_size);
         ddsDictLoad(dds_log_dict, &dds_log_dict->load,
            NULL, "\\", 1);
         start = next;
      }
   }
 
   value_size = next - value;
   if(! value_size || (quote1 && ! quote2)) {
      /* append opening quote */
      quote1 = DDS_TRUE;
      ddsDictLoad(dds_log_dict, &dds_log_dict->load,
         NULL, "\"", 1);
   }
   token_size = next - start;
   if(token_size) ddsDictLoad(dds_log_dict, &dds_log_dict->load,
      NULL, start, token_size);
   if(quote1 || quote2) {
      /* append closing quote */
      ddsDictLoad(dds_log_dict, &dds_log_dict->load,
         NULL, "\"", 1);
   }

   return;
}
