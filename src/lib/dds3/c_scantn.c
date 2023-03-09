static char rcsid[]="$Id: c_scantn.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, select a scan definition and retrieve a specific token.

*/

/***********************************/
/***   Copyright (C) 2003        ***/
/***   BP America, Houston, TX   ***/
/***   Jerry Ehlers, Oct 2003    ***/
/***********************************/

#include "dds_sys.h"

int cdds_scantn(
   const char *alias,
   const char **value,
   int count)
{
   int i, status = DDS_OK;
   size_t save_size;
   const DDS_DEFN *defn;
   DDS_DICT *dict;
   DDS_ALIAS *list;

   DDS_API_ENTRY("scantn")

   if(alias) while(isspace(*alias)) alias++;

   if(alias && *alias) {
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
      status = EOF;
      for(i=0;i<count;i++) {
         status = ddsToken(&dds_scan_token, NULL);
         if(status == EOF) break;
      }
      if(status != EOF) *value = dds_scan_token.str.ptr;
   } else {
      status = EOF;
   }

   DDS_API_EXIT(status);
}
