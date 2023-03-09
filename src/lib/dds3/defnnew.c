static char rcsid[]="$Id: defnnew.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create new definition within dictionary.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define NOM_DEFN_MAX 1024

DDS_DEFN * ddsDefnNew(
   DDS_DICT *dict,
   size_t start,
   size_t alias_size,
   const char *name)
{
   DDS_DEFN *defn, *prev_defn;
   size_t bucket;

   /* recycle the old "defn_eof" for a genuine (named) defn */
   defn = (DDS_DEFN*)dict->defn.newest;
   defn->hash = ddsHash(name);
   defn->name = ddsTablePushStr(&dict->defn, name);
   bucket = defn->hash % dict->defn.size;
   dict->defn.count++;
   if(dict->defn.chain[bucket]) {
      /* add new defn to existing bucket chain */
      for(prev_defn = (DDS_DEFN*)dict->defn.chain[bucket];
         prev_defn;
         prev_defn = (DDS_DEFN*)prev_defn->chain) {
         if(defn->hash == prev_defn->hash &&
            ! strcmp(name, prev_defn->name)) break; /* non-unique */
      }
      if(! prev_defn) dict->defn.unique++;
      defn->chain = dict->defn.chain[bucket];
   } else {
      /* new item is initial entry in bucket chain */
      dict->defn.load++;
      dict->defn.unique++;
   }
   dict->defn.chain[bucket] = (DDS_ITEM*)defn;
   defn->start = start;
   defn->alias = alias_size;
   defn->line = dict->black_line;
   defn->status = 0;

   /* create a new defn (fake for eof) */
   ddsTablePushItem(&dict->defn, NULL, sizeof(DDS_DEFN));

   return defn;
}
