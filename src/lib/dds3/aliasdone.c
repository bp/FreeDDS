static char rcsid[]="$Id: aliasdone.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, cleanup entire definition alias list.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsAliasDone(
   DDS_ALIAS *list)
{
   DDS_ALIAS *alias;

   if(! list) return;

   for(alias = list; alias->right; alias = alias->right) /* null */ ;

   alias->right = dds_alias_done;
   dds_alias_done = list;

   return;
}
