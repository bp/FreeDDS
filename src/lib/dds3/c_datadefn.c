static char rcsid[]="$Id: c_datadefn.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, create filename for binary output, using dictionary rules

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * cdds_datadefn(
   const char *dict,
   const char *out_data)
{
   static DDS_STRING alias = NULL_STRING;
   static DDS_STRING stem_name = NULL_STRING;
   DDS_DICT *out_dict;
   const char *value;

   /* scan the output dictionary */
   cdds_dict(dict, "scan");

   /* Remember the output dictionary. */
   out_dict = dds_scan_first;

   if (out_data) {
      ddsStrCpyCat3(&alias, DDS_SENTRY, " ", out_data);
   } else {
      ddsStrCpy(&alias, DDS_SENTRY);
   }

   if(0 < cdds_scant(alias.ptr, &value) &&
      strcmp(dds_scan_name, DDS_SENTRY)) {
      /* explicit out_data defined for data */
      cdds_dict(dict, "print");
      cdds_printf("$data", " %s\n", out_data);
   } else {
      /* Construct filename(s) from 2 components, i.e. */
      /*    <stem_name> + <data_suffix> */

      if(0 < cdds_scant(dds_data_path.ptr, &value)) {
         ddsStrCpy(&stem_name, value);
      } else {
         ddsStrCpy(&stem_name, dds_cmd_cwd);
      }

#if defined(_WIN32) || defined(MINGW)
      if (stem_name.ptr[strlen(stem_name.ptr) - 1] != '\\') {
          ddsStrCat(&stem_name, "\\");
      }

      value = strrchr(out_dict->name, '\\');
#else
      if(stem_name.ptr[strlen(stem_name.ptr) - 1] != '/') {
         ddsStrCat(&stem_name, "/");
      }

      value = strrchr(out_dict->name, '/');
#endif
      if(value) {
         /* Dictionary name contains a path prefix. */
         ddsStrCat(&stem_name, ++value);
      } else {
         /* Simple dictionary name, without any path prefix. */
         value = out_dict->name;
         ddsStrCat(&stem_name, out_dict->name);
      }

      if(0 <= cdds_scant(dds_data_suffix.ptr, &value)) {
         /* Explicit suffix for filename. */
         ddsStrCat(&stem_name, value);
      } else {
         /* Default suffix for filename. */
         ddsStrCat(&stem_name, "_@");
      }

      value = stem_name.ptr;

      cdds_dict(dict, "print");
      cdds_printf("data", "%s\n", value);
   }

   return value;
}
