static char rcsid[]="$Id: aliaslist.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, update alias list, given string of definition names.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_ALIAS * ddsAliasList(
   DDS_ALIAS *drop,
   const char *alias)
{
   int test;
   size_t size;
   DDS_ALIAS *list, *check;

   /* drop one alias, if it exists */
   list = ddsAliasDrop(drop);

   /* add definition names to alias list */
   while(*alias) {
      /* skip white space */
      while(isspace(*alias)) alias++;

      /* alias name size */
      for(size = 0;
         DEFN_SYNTAX(alias[size]);
         size++) /* NULL */ ;

      /* validate defn name syntax */
      test = alias[size];
      if(test == '=' && ! alias[size + 1]) {
         /* control defn (deprecate) */
         assert(! "scan alias control defn (deprecate)");
      } else if(test && ! isspace(test)) {
         dds_error = EINVAL;
         ddsError("scan: definition name syntax (%s)\n", ddsHint(alias));
         ddsAliasDone(list);
         return NULL;
      } else if(! size) {
         break;
      }
      for(check = list; check; check = check->right) {
         if(size == check->size &&
            ! memcmp(alias, check->name.ptr, size)) {
            /* alias already in the list */
            break;
         }
      }
      if(! check) {
         /* unique alias, add name to list */
         list = ddsAliasAdd(list, alias, size);
      }
      alias += size;
   }

   return list;
}
