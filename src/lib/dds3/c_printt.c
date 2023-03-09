static char rcsid[]="$Id: c_printt.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, append a definition and next token

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_printt(
   const char *alias,
   const char *value)
{
   int itest;
   const char *test;
   const char *start, *next;
   DDS_BOOLEAN quote1 = DDS_FALSE, quote2 = DDS_FALSE;
   size_t value_size = 0, token_size = 0;
   DDS_DEFN *newest_defn;

   DDS_API_ENTRY("printt")

   if(! dds_print_first) {
      DDS_API_EXIT(EOF);
   }

   if(alias) while(isspace(*alias)) alias++;

   if(alias && *alias) {
      /* validate syntax of definition name */
      alias = ddsDefnAxis(alias, dds_print_first);
      for(test=alias; DEFN_SYNTAX(*test); test++) /* null */ ;

      while(isspace(*test)) test++;

      if(*test == '=' && test[1] == '\0') {
         /* control defn (deprecate) */
         assert(! "cdds_print control defn (deprecate)");
      } else if(*test) {
         dds_error = EINVAL;
         ddsError("print: definition name syntax (%s)\n\tdictionary= %s\n",
            ddsHint(alias), dds_print_first->name);
         DDS_API_EXIT(EOF);
      }

      /* append new definition name */
      newest_defn = (DDS_DEFN*)dds_print_first->defn.newest;
      if(newest_defn->start &&
         ! isspace(dds_print_first->buf.ptr
            [newest_defn->start - 1])) {
         /* insert '\n' (white space) name prefix */
         ddsDictLoad(dds_print_first, &dds_print_first->load,
            NULL, "\n", 1);
      }
      ddsDictLoad(dds_print_first, &dds_print_first->load,
         NULL, alias, (test - alias));
      ddsDictLoad(dds_print_first, &dds_print_first->load,
         NULL, "=", 1);
   }

   /* convert value string into definition token */
   ddsDictLoad(dds_print_first, &dds_print_first->load,
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
            ddsDictLoad(dds_print_first, &dds_print_first->load,
               NULL, "\"", 1);
            quote2 = DDS_TRUE;
         }
         token_size = next - start;
         if(token_size)
            ddsDictLoad(dds_print_first, &dds_print_first->load,
               NULL, start, token_size);
         ddsDictLoad(dds_print_first, &dds_print_first->load,
            NULL, "\\", 1);
         start = next;
      }
   }

   value_size = next - value;
   if(! value_size || (quote1 && ! quote2)) {
      /* append opening quote */
      quote1 = DDS_TRUE;
      ddsDictLoad(dds_print_first, &dds_print_first->load,
         NULL, "\"", 1);
   } 
   token_size = next - start;
   if(token_size) ddsDictLoad(dds_print_first, &dds_print_first->load,
      NULL, start, token_size);
   if(quote1 || quote2) {
      /* append closing quote */
      ddsDictLoad(dds_print_first, &dds_print_first->load,
         NULL, "\"", 1);
   }


   DDS_API_EXIT(value_size);
}
