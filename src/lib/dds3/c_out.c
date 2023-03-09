static char rcsid[]="$Id: c_out.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, initialize dictionary for binary output data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * cdds_out(
   const char *alias,
   const char *value,
   const char *history)
{
   static DDS_STRING out_data = NULL_STRING;
   static DDS_TOKEN out_save = NULL_TOKEN;
   static DDS_TOKEN out_tmp = NULL_TOKEN;
   const char *out_alias, *out_value, *out_dict, *out_prefix;
   DDS_DICT *dict;

   DDS_API_ENTRY("out")

   cdds_dict("par:", "scan");

   if(alias) while(isspace(*alias)) alias++;

   if(alias && *alias) {
      /* definition alias for the dictionary name */
      out_alias = ddsString(TOKEN_INIT(out_tmp, alias));
   } else {
      out_alias = NULL;
   }

   if(value) while(isspace(*value)) value++;

   if(alias && *alias && ! cdds_scanf(alias, "")) {
      /* output dictionary explicitly named by "alias" */
      out_value = NULL;
      cdds_scanv("", &out_value);
      if(out_value) while(isspace(*out_value)) out_value++;
   } else if(value && *value) {
      /* non-trivial default provided */
      if(strstr(value, "stdout:")) {
         /* caution: "value" uses "stdout:".
            "value" should not be used if "par:" defines data,
            because "stdout:" may be needed for binary data. */
         if(alias && *alias) {
            /* search "par:" for "xxx_data" */
            out_value = value; /* assume search failure */
            ddsStrSetMin(&out_save.str, "", DEFNNAME_MAX);
            ddsStrSetMin(&out_data, "", DEFNNAME_MAX);
            for(out_prefix = TOKEN_INIT(out_save, alias);
               out_prefix;
               out_prefix = TOKEN_NEXT(out_save)) {
               ddsStrCpyCat2(&out_data, out_prefix, "_data");
               if(! cdds_scanf(out_data.ptr, "")) {
                  /* ignore "value" because "par:" defines "out_data" */
                  out_value = NULL;
                  break; 
               }
            }
            ddsStrFree(&out_save.str);
            ddsStrFree(&out_data);
         } else {
            out_value = value;
         }
      } else {
         out_value = value;
      }
   } else {
      out_value = NULL;
   }

   if(! out_value || ! *out_value) {
      out_dict = ddsString(ddsTmpDict("_out"));
   } else {
      if(! strcmp(out_value, "stdin:") ||
         ! strcmp(out_value, "cmd:") ||
         ! strcmp(out_value, "par:") ||
         ! strcmp(out_value, "env:")) {
         out_dict = ddsString(ddsTmpDict("_out"));
         ddsError("invalid name for output dictionary (%s)\n\ttmp dictionary= %s\n",
            out_value, out_dict);
      } else {
         out_dict = ddsString(out_value);
         dict = (DDS_DICT*)ddsTableLookup(&dds_dict, out_dict);
         if(dict) {
            ddsDebug("out dictionary already opened\n\tdictionary= %s\n",
               out_dict);
            cdds_dict(out_dict, "print");
            DDS_API_EXIT(out_dict);
         }
      }
   }
   cdds_dict(history, "scan");
   cdds_dict(out_dict, "print");
   cdds_history();

   if((! out_value || ! *out_value) && (dds_state & DDS_SAVE_TMP)) {
      ddsVerbose("out dictionary '%s', real filename %s\n",
         dds_print_first->name, dds_print_first->media->name);
   }

   dds_print_first->alias = out_alias;

   DDS_API_EXIT(out_dict);
}
