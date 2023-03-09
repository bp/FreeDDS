static char rcsid[]="$Id: defnfind.c 124 2009-10-26 15:47:00Z user $";
/*
RCS-t=DDS, find definition, given dictionary and alias list

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define ALIAS_LIST_MAX 32

const DDS_DEFN * ddsDefnFind(
   size_t *save_size,
   size_t *save_bias,
   DDS_ALIAS **save_list,
   const DDS_DICT *dict)
{
   static DDS_STRING defn_value = NULL_STRING;
   DDS_ALIAS *alias, *defn_alias, *list = *save_list;
   DDS_DEFN *defn, *test_defn, *defn_newer;
   const char *defn_start;
   size_t bucket, index_min, index_max;
   size_t defn_bias;

   index_max = ((DDS_DEFN*)dict->defn.newest)->start;
   index_min = 0;
   defn = NULL;

   /* search for a definition, while an alias list exists */
   while(list) {

      /* for each definition alias in the list */
      for(alias = list; alias; alias = alias->right) {

         /* for each bucket in the hash chain */
         bucket = alias->hash % dict->defn.size;
         for(test_defn = (DDS_DEFN*)dict->defn.chain[bucket];
            test_defn;
            test_defn = (DDS_DEFN*)test_defn->chain) {
            if(test_defn->start < index_min) {
               break;                                       /* too old */
            }
            if(alias->hash != test_defn->hash ||            /* wrong hash */
               test_defn->start >= index_max ||             /* too new */
               strcmp(alias->name.ptr, test_defn->name))    /* spelling */
               continue;  /* try next bucket */

            /* MATCH (alias and defn), save vital statistics */
            defn = test_defn;
            defn_alias = alias;
            defn_start = dict->buf.ptr + defn->start + defn->alias;
            defn_bias = strchr(defn_start, '=') - dict->buf.ptr + 1;

            /* update age constraint (newer than this definition) */
            index_min = test_defn->start;
            break;
         }
      }

      if(! defn) {
         /* DEFINITION NOT FOUND, try another dictionary */
         break;
      }

      /* DEFINITION FOUND */

      if((! defn->alias && ! defn_alias->alias) || /* normal */
         (defn->alias && defn_alias->alias)) {     /* alias to alias */
         /* DEFINITION FOUND AND MATCHED */
         *save_size = defn_alias->size;
         *save_bias = defn_bias;
         *save_list = NULL;
         ddsAliasDone(list);
         return defn;
      }

      /* ALIAS DEFINITION, update alias list and continue search */
      ddsParTag(defn, dict);
      defn_newer = (DDS_DEFN*)defn->newer;
      ddsStrCpyN(&defn_value, dict->buf.ptr + defn_bias,
         (defn_newer->start - defn_bias));
      list = ddsAliasList(NULL, defn_value.ptr);
      defn = NULL;
      index_max = index_min;
      index_min = 0;
   }

   *save_size = 0;
   *save_bias = 0;
   *save_list = list;

   return NULL;
}
