static char rcsid[]="$Id: c_mod.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, initialize dictionary for binary data (modify)

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * cdds_mod(
   const char *alias,
   const char *value,
   const char *title,
   const char *history)
{
   static DDS_TOKEN mod_tmp = {{0, NULL}, NULL};
   const char *mod_alias, *mod_value, *mod_dict;
   DDS_DICT *dict;
   DDS_BOOLEAN preserve;
   STRUCTSTAT stat_buf;

   DDS_API_ENTRY("mod")

   cdds_dict("par:", "scan");

   if(alias) while(isspace(*alias)) alias++;

   if(alias && *alias) {
      /* definition alias for the dictionary name */
      mod_alias = ddsString(TOKEN_INIT(mod_tmp, alias));
   } else {
      mod_alias = NULL;
   }

   if(value) while(isspace(*value)) value++;

   if(alias && *alias && ! cdds_scanf(alias, "")) {
      /* output dictionary explicitly named by "alias" */
      mod_value = NULL;
      cdds_scant("", &mod_value);
      if(mod_value) while(isspace(*mod_value)) mod_value++;
   } else if(value && *value) {
      /* non-trivial default provided */
      if(strstr(value, "stdin:") ||
         strstr(value, "stdout:")) {
         ddsDebug("default dictionary uses stdin: or stdout:\n");
         mod_value = NULL;
      } else {
         mod_value = value;
      }
   } else {
      mod_value = NULL;
   }




   if(! mod_value || ! *mod_value) {
      mod_dict = ddsString(ddsTmpDict("_mod"));
      ddsWarn("mod dictionary not specified\n\ttmp dictionary= %s\n",
         mod_dict);
      preserve = DDS_FALSE;
   } else {
      if(! strcmp(mod_value, "stdin:") ||
         ! strcmp(mod_value, "stdout:") ||
         ! strcmp(mod_value, "cmd:") ||
         ! strcmp(mod_value, "par:") ||
         ! strcmp(mod_value, "env:")) {
         mod_dict = ddsString(ddsTmpDict("_out"));
         ddsError("invalid name for output dictionary (%s)\n\ttmp dictionary= %s\n",
            mod_value, mod_dict);
         preserve = DDS_FALSE;
      } else {
         mod_dict = ddsString(mod_value);
         dict = (DDS_DICT*)ddsTableLookup(&dds_dict, mod_dict);
         if(dict) {
            mod_dict = ddsString(ddsTmpDict("_mod"));
            ddsDebug("mod dictionary already opened\n\tdictionary= %s\n",
               mod_value);
            cdds_dict(mod_dict, "print");
            DDS_API_EXIT(mod_dict);
         } else if(! ddsSTAT(mod_dict, &stat_buf) && S_ISREG(stat_buf.st_mode)) {
            preserve = DDS_TRUE;
         } else {
            preserve = DDS_FALSE;
         }
      }
   }

   if(preserve) {
      /* preserve existing contents */
      cdds_dict(mod_dict, "scan");
      /* update dictionary with title and par: dictionary */
      cdds_dict(mod_dict, "print");
      ddsTitle(title);
      /* SEP uses "in" for the binary data filename */
      cdds_printf("$sep_in", " in\n");
      cdds_dict("par:", "scan");
      cdds_history();
   } else {
      /* initialize dictionary using hist */
      cdds_dict(history, "scan");
      cdds_dict(mod_dict, "print");
      cdds_history();
   }

   if((! mod_value || ! *mod_value) && (dds_state & DDS_SAVE_TMP)) {
      ddsVerbose("mod dictionary '%s', real filename %s\n",
         dds_print_first->name, dds_print_first->media->name);
   }

   cdds_dict(mod_dict, "scan");

   dds_scan_first->alias = mod_alias;

   DDS_API_EXIT(mod_dict);
}
