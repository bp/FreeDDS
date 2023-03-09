static char rcsid[]="$Id: f_scanf.c 118 2009-09-29 19:46:58Z user $";
/*
RCS-t=DDS API, select a scan definition and retrieve formatted value.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define FMT_NOM_LEN 2 * DDS_KB

#define ALIAS_NOM_LEN 4 * DEFNNAME_MAX

HOST_FTN_INT
HOST_FTN_FUNC(fdds_scanf, FDDS_SCANF)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_STR fmt_ftn,
   ...)
{
   static DDS_STRING alias_str = NULL_STRING;
   static DDS_STRING fmt_str = NULL_STRING;

   const char *alias;
   int status = DDS_OK;
   size_t save_size;
   const DDS_DEFN *defn;
   DDS_DICT *dict;
   DDS_ALIAS *list;
   va_list ap, ap_len;

   HOST_FTN_LEN alias_len, fmt_len;

   DDS_API_ENTRY("scanf")

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
   status = ddsScanfF(&dds_scan_token.next, &ap_len, NULL, fmt_str.ptr,
      "<fake>", "<fake>");
   if(status == EOF) {
      va_end(ap);
      DDS_API_EXIT(EOF);
   }

   /* length of "alias" character argument */
   alias_len = va_arg(ap_len, HOST_FTN_LEN);

   /* length of "fmt" character argument */
   fmt_len = va_arg(ap_len, HOST_FTN_LEN);
   if (fmt_len < strlen(fmt_str.ptr)) {
      ddsError("fdds_scanf: definition format (%s)\n\tnull termination length=%d > length=%d\n",
               fmt_str.ptr, strlen(fmt_str.ptr), fmt_len);
      va_end(ap);
      DDS_API_EXIT(EOF);
   }

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   alias = alias_str.ptr;

   while(isspace(*alias)) alias++;

   if(*alias) {
      /* de-select current scan definition */
      ddsScanDone();

      /* initialize alias list (definition names) */
      list = ddsAliasList(NULL, ddsDefnAxis(alias, dds_scan_first));

      /* search for a defn, while dict and alias exist */
      for(dict = dds_scan_first; dict && list; dict = dict->right) {
         defn = ddsDefnFind(&save_size, &dds_scan_value_bias,
            &list, dict);
         if(defn) {
            /* DEFINITION FOUND */
            dds_scan_dict = dict;
            dds_scan_defn = defn;
            dds_scan_name = defn->name + defn->alias;
            dds_scan_newer = (DDS_DEFN*)defn->newer;
            dds_scan_value_save = dict->buf.ptr[dds_scan_newer->start];
            dict->buf.ptr[dds_scan_newer->start] = '\0';
            dds_scan_token.next = dict->buf.ptr + dds_scan_value_bias;
            break;
         }
      }
      ddsAliasDone(list);
   }
   if(dds_scan_dict) {
      ddsParTag((DDS_DEFN*)dds_scan_defn, dds_scan_dict);
      status = ddsScanfF(&dds_scan_token.next, &ap, &ap_len, fmt_str.ptr,
         dds_scan_dict->name, dds_scan_name);
   } else {
      status = EOF;
   }

   va_end(ap);

   DDS_API_EXIT(status);
}
