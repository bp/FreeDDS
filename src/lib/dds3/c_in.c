static char rcsid[]="$Id: c_in.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, initialize dictionary for binary input data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * cdds_in(
   const char *alias,
   const char *value,
   const char *title)
{
   static DDS_TOKEN in_list = {{0, NULL}, NULL};
   static DDS_TOKEN in_tmp = {{0, NULL}, NULL};
   const char *in_alias, *in_value, *in_history, *in_prefix, *in_dict;
   DDS_DICT *dict;

   DDS_API_ENTRY("in")

   cdds_dict("par:", "scan");

   if(! in_list.str.max)
      ddsStrSetMin(&in_list.str, "", (2 * PATHNAME_MAX));

   if(! in_tmp.str.max)
      ddsStrSetMin(&in_tmp.str, "", (2 * PATHNAME_MAX));

   if(alias) while(isspace(*alias)) alias++;

   if(alias && *alias) {
      /* definition alias for the dictionary name */
      in_alias = ddsString(TOKEN_INIT(in_tmp, alias));
   } else {
      in_alias = NULL;
   }

   if(value) while(isspace(*value)) value++;

   if(alias && *alias && ! cdds_scanf(alias, "")) {
      /* input history explicitly named by "alias" */
      in_value = "";
      cdds_scanv("", &in_value);
      while(isspace(*in_value)) in_value++;
      if(*in_value) {
         ddsStrCpy(&in_tmp.str, in_value);
         in_history = in_tmp.str.ptr;
      } else {
         in_history = NULL;
      }
   } else if(value && *value) {
      /* non-trivial default provided */
      if(strstr(value, "stdin:")) {
         /* caution: "value" uses "stdin:".
            "value" should not be used if "par:" defines format,
            because "stdin:" may be needed for binary data. */
         if(! cdds_scanf("format", "")) {
            /* ignore "value" because "par:" defines "format" */
            in_history = NULL;
         } else if(alias && *alias) {
            /* search "par:" for "xxx_format" */
            in_history = value; /* assume search failure */
            for(in_prefix = TOKEN_INIT(in_tmp, alias);
               in_prefix;
               in_prefix = TOKEN_NEXT(in_tmp)) {
               ddsStrCpyCat2(&in_list.str, in_prefix, "_format");
               if(! cdds_scanf(in_list.str.ptr, "")) {
                  /* ignore "value" because "par:" defines "in_format" */
                  in_history = NULL;
                  break; 
               }
            }
         } else {
            in_history = value;
         }
      } else {
         in_history = value;
      }
   } else {
      in_history = NULL;
   }

   ddsStrCpy(&in_list.str, ddsTmpDict("_in"));

   if(in_history) {
      cdds_dict(in_history, "scan");
      for(dict = dds_scan_first; dict; dict = dict->right)
         ddsStrCat2(&in_list.str, " ", dict->name);
   }

   in_dict = ddsString(in_list.str.ptr);

   cdds_dict(in_dict, "print");

   ddsTitle(title);

   /* SEP uses "in" for the binary data filename */
   cdds_printv("alias:sep_in", " in\n");

   /* Deprecate size to avoid transpose problems */
   cdds_printv("$size", "\n");
   cdds_printv("$origin", "\n");
   cdds_printv("$delta", "\n");
   cdds_printv("$units", "\n");

   cdds_dict("par:", "scan");
   cdds_history();

   cdds_dict(in_dict, "scan");

   dds_scan_first->alias = in_alias;

   ddsStrFree(&in_list.str);
   ddsStrFree(&in_tmp.str);

   DDS_API_EXIT(in_dict);
}
