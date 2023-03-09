static char rcsid[]="$Id: c_printf.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, append a definition and formatted value

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_printf(
   const char *alias,
   const char *fmt,
   ...)
{
   const char *test;
   int value_size;
   DDS_DEFN *newest_defn;
   va_list ap;

   DDS_API_ENTRY("printf")

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

   /* append definition value */
   va_start(ap, fmt);
   value_size = ddsPrintfC(dds_print_first, &ap, fmt);
   va_end(ap);

   DDS_API_EXIT(value_size);
}
