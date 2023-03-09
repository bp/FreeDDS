static char rcsid[]="$Id: aliasadd.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, add alias to definition alias list.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define NOM_ALIAS_MAX 64

DDS_ALIAS * ddsAliasAdd(
   DDS_ALIAS *list,
   const char *name,
   size_t size)
{
   int n;
   DDS_ALIAS *alias;
   size_t alias_count;

   /* vacant definition descriptor */
   alias = dds_alias_done;

   if(! alias) {
      /* allocate more alias descriptors */
      alias_count = dds_alias_count ? dds_alias_count : NOM_ALIAS_MAX;
      alias = (DDS_ALIAS*)ddsMalloc(alias_count * sizeof(DDS_ALIAS));
      for(n = 0; n < alias_count; n++) {
         alias[n].right = alias + n + 1;
         ddsStrSetMin(&alias[n].name, "", DDS_DEFN_NAME_MAX);
      }
      alias[alias_count - 1].right = NULL;
      dds_alias_count += alias_count;
      alias->right = dds_alias_free;
      dds_alias_free = alias++;
   }

   dds_alias_done = alias->right;

   if(*name == '$') {
      alias->alias = DDS_TRUE;
      name++;
      size--;
   } else if(size > 5 && ! memcmp(name, "alias:", 6)) {
      alias->alias = DDS_TRUE;
      name += 6;
      size -= 6;
   } else {
      alias->alias = DDS_FALSE;
   }
   ddsStrCpyN(&alias->name, name, size);
   alias->hash = ddsHash(alias->name.ptr);
   alias->size = size;

   if(list) {
      alias->left = list->left;
      if(list->left) list->left->right = alias;
      alias->right = list;
      if(list) list->left = alias;
      /* normalize alias list pointer */
      while(alias->left) alias = alias->left;
   } else {
      alias->left = alias->right = NULL;
   }

   return alias;
}
