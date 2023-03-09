static char rcsid[]="$Id: f_printf.c 117 2009-09-29 19:26:22Z user $";
/*
RCS-t=DDS API, append a definition and formatted value

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define ALIAS_NOM_LEN 4 * DEFNNAME_MAX

#define FMT_NOM_LEN 2 * DDS_KB


HOST_FTN_INT
HOST_FTN_FUNC(fdds_printf, FDDS_PRINTF)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_STR fmt_ftn,
   ...)
{
   static DDS_STRING alias_str = NULL_STRING;
   static DDS_STRING fmt_str = NULL_STRING;

   const char *alias, *test;
   int value_size;
   DDS_DEFN *newest_defn;
   va_list ap, ap_len;

   HOST_FTN_LEN alias_len, fmt_len;

   DDS_API_ENTRY("printf")

   if(! dds_print_first) {
      DDS_API_EXIT(EOF);
   }

   FTN_FMT_TO_STR(fmt_str, fmt_ftn);

   va_start(ap, fmt_ftn);
   /* NOTE: explaination of the following insane kludge...
      The length information for Fortran character arguments
      appears in the argument list following the normal pointers.
      "ap_len" contains an independent copy of va_list info.
      It must be walked pasted the normal pointers,
      using the format '%' conversion specifications.
      The format and argument conversion can then proceed,
      using the original "ap" (pointers to actual args) and
      "ap_len" (length of Fortran character args).
   */

   /* Walk "ap_len" past the pointers to actual args */
   memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
   value_size = ddsPrintfF(dds_print_first, &ap_len, NULL, fmt_str.ptr);
   if(value_size == EOF) {
      va_end(ap);
      DDS_API_EXIT(EOF);
   }

   /* length of "alias" character argument */
   alias_len = va_arg(ap_len, HOST_FTN_LEN);

   /* length of "fmt" character argument */
   fmt_len = va_arg(ap_len, HOST_FTN_LEN);
   if (fmt_len < strlen(fmt_str.ptr)) {
      ddsError("fdds_printf: definition format (%s)\n\tnull termination length=%d > length=%d\n",
               fmt_str.ptr, strlen(fmt_str.ptr), fmt_len);
      va_end(ap);
      DDS_API_EXIT(EOF);
   }
      

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   alias = alias_str.ptr;

   while(isspace(*alias)) alias++;

   if(*alias) {
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
         va_end(ap);
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
   value_size = ddsPrintfF(dds_print_first, &ap, &ap_len, fmt_str.ptr);
   va_end(ap);

   DDS_API_EXIT(value_size);
}
