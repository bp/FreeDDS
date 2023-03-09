static char rcsid[]="$Id: c_newer.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, find newer definition, given alias list

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 19 Aug 1996            ***/
/**********************************************/

#include "dds_sys.h"

const char * cdds_newer(
   const char *alias)
{
   DDS_ALIAS *list;
   const DDS_DEFN *defn;
   DDS_DICT *dict;
   const char *defn_start;

   DDS_API_ENTRY("newer")

   /* get starting definition */
   dict = dds_scan_dict;
   defn = dds_scan_defn;

   /* initialize scan dictionary, if needed */
   if (! dict) {
      if (! dds_scan_last) { DDS_API_EXIT(NULL); }
      dict = dds_scan_last;
      defn = NULL;
   }

   /* initilaize scan definition, if needed */
   if (! defn) {
      defn = (DDS_DEFN*)dict->defn.oldest;
      if (! defn) { DDS_API_EXIT(NULL); }
   }
   else if (defn->newer) {
      defn = (DDS_DEFN*)defn->newer;
   }

   while (! defn->newer) {
      dict = dict->left;
      if (! dict) { DDS_API_EXIT(NULL); }
      defn = (DDS_DEFN*)dict->defn.oldest;
   }

   /* de-select current scan definition */
   ddsScanDone();
      
   /* if no alias given, return next newer definition */     
   if (alias) while(isspace(*alias)) alias++;
   if (! alias || ! *alias) {
      dds_scan_dict = dict;
      dds_scan_defn = defn;
      defn_start = dict->buf.ptr + defn->start + defn->alias;
      dds_scan_value_bias = strchr(defn_start, '=') - dict->buf.ptr + 1;
      dds_scan_name = defn->name;
      dds_scan_newer = (DDS_DEFN*)defn->newer;
      dds_scan_value_save = dict->buf.ptr[dds_scan_newer->start];
      dict->buf.ptr[dds_scan_newer->start] = '\0';
      dds_scan_token.next = dict->buf.ptr + dds_scan_value_bias;
 
      if (! dds_scan_name) { DDS_API_EXIT(NULL); }
      if (! strlen(dds_scan_name)) { DDS_API_EXIT(NULL); }
      DDS_API_EXIT(dds_scan_name);
   }

   /* initialize alias list (definition names) */
   list = ddsAliasList(NULL, ddsDefnAxis(alias, dict));

   /* search for a definition, while dict exists */
   dds_scan_name = NULL;
   while(dict) {
      defn = ddsDefnNewer(&dds_scan_value_bias, &list, dict, defn);
      if(defn) {
         /* DEFINITION FOUND */
         dds_scan_dict = dict;
         dds_scan_defn = defn;
         dds_scan_name = defn->name;
         dds_scan_newer = (DDS_DEFN*)defn->newer;
         dds_scan_value_save = dict->buf.ptr[dds_scan_newer->start];
         dict->buf.ptr[dds_scan_newer->start] = '\0';
         dds_scan_token.next = dict->buf.ptr + dds_scan_value_bias;
         break;
      }
      dict = dict->left;
      if (dict) defn = (DDS_DEFN*)dict->defn.oldest;
   }

   ddsAliasDone(list);
 
   if (! dds_scan_name) { DDS_API_EXIT(NULL); }
   if (! strlen(dds_scan_name)) { DDS_API_EXIT(NULL); }
   DDS_API_EXIT(dds_scan_name);
}
