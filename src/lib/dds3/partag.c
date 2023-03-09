static char rcsid[]="$Id: partag.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, tag definition in par dictionary to know if ever used

*/

/***********************************/
/***   Copyright (C) 2005        ***/
/***   BP America, Houston, TX   ***/
/***   Jerry Ehlers, Feb 2005    ***/
/***********************************/

#include "dds_sys.h"

static int dds_par_check = 1;

void ddsParCheck(int check) { dds_par_check = check; }

void ddsParTag(
   DDS_DEFN *defn,
   const DDS_DICT *dict)
{
   DDS_ALIAS *alias;
   DDS_DEFN *test_defn;
   DDS_DICT *pardict;
   size_t bucket;

   if (!dds_par_check) return;

   defn->status |= 1;

   /* Find the definition in the par file to tag as found */
   if (strcmp(dict->name, "par:") && strcmp(dict->name, "override:")) {
      ddsVerbose("ParTag: dict '%s' defn '%s'\n", dict->name, defn->name);

      /* Find the par: dict */
      pardict = (DDS_DICT*)ddsTableLookup(&dds_dict, "par:");
      if (!pardict) return;

      /* Find the defn in par: */
      alias = ddsAliasList(NULL, defn->name);
      bucket = alias->hash % pardict->defn.size;
      for(test_defn = (DDS_DEFN*)pardict->defn.chain[bucket];
          test_defn;
          test_defn = (DDS_DEFN*)test_defn->chain) {
         if(alias->hash != test_defn->hash ||            /* wrong hash */
            strcmp(alias->name.ptr, test_defn->name))    /* spelling */
            continue;  /* try next bucket */

         /* MATCH (alias and defn), save vital statistics */
         ddsVerbose("ParTag: Tag defn found\n");
         test_defn->status |= 1;
         break;
      }
   }
}
