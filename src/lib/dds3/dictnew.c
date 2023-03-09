static char rcsid[]="$Id: dictnew.c 112 2009-09-03 22:10:35Z user $";
/*
RCS-t=DDS, malloc and default DDS_DICT (dictionary hash bucket).

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

/*
Musa Maharramov 260809

Dropped tmpnam()
Switched to gopen_tempfile() from libgio for temp file creation
*/

#include "dds_sys.h"


DDS_DICT * ddsDictNew(
   const char *name,
   DDS_DICT_STATE dict_state,
   DDS_MEDIA_STATE media_state,
   const char *media_name,
   GFILE *media_fp,
   char *buf_str,
   size_t buf_size)
{
   DDS_AUTO_FMT detect;
   DDS_DICT *dict, *prev_dict, *dict_older, *dict_newer;
   size_t bucket;
   const char *tmp_name,*str;


   name = ddsString(name);

   for(dict = (DDS_DICT*)dds_dict.oldest;
      dict;
      dict = (DDS_DICT*)dict->newer) {
      if(! (dict->state & DDS_DICT_ACTIVE)) {
         /* reuse old entry */
         dict_older = (DDS_DICT*)dict->older;
         dict_newer = (DDS_DICT*)dict->newer;
         memset((void*)dict, '\0', sizeof(DDS_DICT));
         dict->hash = ddsHash(name);
         dict->name = name;
         dict->older = (DDS_ITEM*)dict_older;
         dict->newer = (DDS_ITEM*)dict_newer;
         bucket = dict->hash % dds_dict.size;
         dds_dict.count++;
         if(dds_dict.chain[bucket]) {
            /* add new dict to existing bucket chain */
            for(prev_dict = (DDS_DICT*)dds_dict.chain[bucket];
               prev_dict;
               prev_dict = (DDS_DICT*)prev_dict->chain) {
               if(dict->hash == prev_dict->hash &&
                  ! strcmp(name, prev_dict->name)) break; /* non-unique */
            }
            if(! prev_dict) dds_dict.unique++;
            dict->chain = dds_dict.chain[bucket];
         } else {
            /* new item is initial entry in bucket chain */
            dds_dict.load++;
            dds_dict.unique++;
         }
         dds_dict.chain[bucket] = (DDS_ITEM*)dict;
         break;
      }
   }

   if(! dict) dict = (DDS_DICT*)ddsTablePushItem(&dds_dict,
      ddsString(name), sizeof(DDS_DICT));

   dict->state = dict_state;

   if(media_fp) {
      /* dictionary has an associated GFILE pointer */
      dict->media =
         ddsMediaNew(dict, NULL, media_state, media_name, media_fp);
   }

   if(buf_str) {
      /* string constant for dictionary buffer (read only, never realloc) */
      dict->buf.max = buf_size;
      dict->buf.ptr = buf_str;
   } else {
      /* dynamic dictionary buffer (read/write/realloc) */
      ddsStrSetMin(&dict->buf, "", (DICT_BUF_SIZE));
   }

   ddsTableInit(&dict->defn, DEFN_HASH_SIZE,
      (DEFN_HASH_ALPHA * DEFN_HASH_SIZE * sizeof(DDS_DEFN)),
      DEFN_HASH_ALPHA, DEFN_HASH_BETA);

   ddsTablePushItem(&dict->defn, NULL, sizeof(DDS_DEFN));

   dict->load.escape = DDS_FALSE;
   dict->load.quote = DDS_FALSE;
   dict->load.name = dict->name;

   ddsStrSetMin(&dict->print_name, "", DDS_DEFN_NAME_MAX);

   if(media_fp && (dict_state & DDS_DICT_LOAD)) {
      /* examine the media byte sequence (dict ? binary ?) */
      ddsAutoFmt(&detect, media_fp);
      if(detect.fmt != DDS_FMT_GENERIC
/*** 21 July 1998, RLS bad idea
&& detect.fmt != DDS_FMT_UNKNOWN
***/
         ) {
         /* binary data detected (fake a dictionary) */
         dict->state |= DDS_DICT_SHAM;
         media_fp = gopen_tempfile(&tmp_name, "cw+");
         if(! media_fp) {
            /* panic (not error), if temporary files can't be accessed */
            ddsPanic("dict: errno= %i, %s\n\topening '%s' (%s) dictionary, mode= w+\n",
               errno, strerror(errno), name, tmp_name);
         } else {
            if(dds_state & DDS_SAVE_TMP) {
               /* retain temporary dictionary */
               ddsVerbose("dictionary '%s', real filename %s\n", name, tmp_name);
            } else {
               /* dispose of the temporary dictionary */
               unlink(tmp_name);
            }
         }

         /* preserve the data filename and format as definitions */
         str = "data= ";
         gwritec(media_fp, str, strlen(str));
         gwritec(media_fp, name, strlen(name));
         str = "\nformat= ";
         gwritec(media_fp, str, strlen(str));
         str = dds_fmt_name[detect.fmt];
         gwritec(media_fp, str, strlen(str));
         gwritec(media_fp, "\n", 1);
         grewind(media_fp);

         /* original media_fp is accessible via dds_media_list */
         dict->media->dict = NULL;

         dict->media =
            ddsMediaNew(dict, NULL, media_state, tmp_name, media_fp);
      }
   }

   dict->state |= DDS_DICT_ACTIVE;

   return dict;
}
