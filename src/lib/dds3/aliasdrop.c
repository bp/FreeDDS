static char rcsid[]="$Id: aliasdrop.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, drop one alias from definition alias list.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_ALIAS * ddsAliasDrop(
   DDS_ALIAS *alias)
{
   DDS_ALIAS *list = NULL;

   if(alias) {
      if(alias->right) alias->right->left = alias->left;
      if(alias->left) alias->left->right = alias->right;
      if(alias->left)
         list = alias->left;
      else
         list = alias->right;

      alias->right = dds_alias_done;
      dds_alias_done = alias;

      /* normalize alias list pointer */
      if(list) while(list->left) list = list->left;
   }

   return list;
}
