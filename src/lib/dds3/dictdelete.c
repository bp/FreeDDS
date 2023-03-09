static char rcsid[]="$Id: dictdelete.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, close and free DDS_DICT (dictionary hash bucket).

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsDictDelete(
   DDS_DICT *dict)
{
   DDS_MEDIA *media, *media2;
   DDS_DICT *dict2, *dict_older;
   int status = DDS_OK;

   if(dict->state & DDS_DICT_SCAN) {
      if(dict == dds_scan_dict) {
         /* de-select current scan definition */
         ddsScanDone();
      }
      if(dict == dds_scan_first) dds_scan_first = dict->right;
      if(dict == dds_scan_last)  dds_scan_last = dict->left;
   } else if(dict->state & DDS_DICT_PRINT) {
      if(dict == dds_print_first) {
         /* de-select all print definition */
         if(ddsPrintDone()) status = DDS_BAD;

         for(dict2 = dict; dict2; dict2 = dict_older) {
            dict_older = dict2->right;
            dict2->state &= (DDS_DICT_STATE) ~ DDS_DICT_PRINT;
            dict2->left = dict2->right = NULL;
         }
         dds_print_first = dds_print_last = NULL;
      } else if(dict == dds_print_last) {
         dds_print_last = dict->left;
      }
   }

   if(dict->left) dict->left->right = dict->right;

   if(dict->right) dict->right->left = dict->left;

   dict->state &= ~ DDS_DICT_ACTIVE;

   ddsTableUnhash(&dds_dict, (DDS_ITEM*)dict);

   if(dict->media) {
      media = dict->media;
      while(media->prev) media = media->prev;

      for(media2 = media; media2; media2 = media2->next) {
         media2->dict = NULL;
      }

      if(! media->bin) {
         if(ddsMediaDelete(media)) status = DDS_BAD;
      }
   }

   if(! (dict->state & DDS_DICT_STR_BUF)) {
      ddsStrFree(&dict->buf);
   }

   ddsTableTerm(&dict->defn);

   ddsStrFree(&dict->print_name);

   /* if closing "log:" file, reset abort/print flags */
   if (!strcmp(dict->name, "log:")) 
   {
      if (dds_state & DDS_CLEANUP) {
         dds_state = (DDS_STATE)(dds_state & ~ (DDS_ERROR_LOG
            | DDS_WARN_ABORT | DDS_WARN_LOG | DDS_DEBUG_ABORT
            | DDS_DEBUG_LOG | DDS_OPERATOR_LOG | DDS_VERBOSE_LOG));

         dds_state = (DDS_STATE)(dds_state | DDS_ERROR_ABORT
            | DDS_ERROR_PRINT | DDS_WARN_PRINT | DDS_DEBUG_PRINT
            | DDS_OPERATOR);
      } else {
         ddsDictLog("print", (DDS_DICT_STATE)DDS_DICT_TRUNCATE,
            (DDS_MEDIA_STATE)(DDS_MEDIA_OPENR | DDS_MEDIA_OPENW));
      }
   }

   if(dict->alias) ddsFree((void*)dict->alias);

   return status;
}
