static char rcsid[]="$Id: aliasname2.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create defn alias names

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsAliasName2(
   DDS_STRING *str,
   const char *mode,
   const char *name1,
   const char *name2)
{
   size_t str_size;

   /*****************************************/
   /*  The syntax of the defn alias is...   */
   /*     "mode:name1.name2"                */
   /*        +  " mode:name1.*"             */
   /*        +  " mode:*.name2"             */
   /*        +  " mode:*.*"                 */
   /*  where "mode" is "read" or "write".   */
   /*  where "name1" is either a dictionary */
   /*     alias ("in", "out"...) or format  */
   /*     name ("usp", "segy"...)           */
   /*  where "name2" is a member name       */
   /*     ("RecNum", "TrcNum" ...)          */
   /*****************************************/

   str_size = 20 + 4 * strlen(mode) + 2 * (strlen(name1) + strlen(name2));

   ddsStrCpyMin(str, "", str_size);

   sprintf(str->ptr, "%s:%s.%s %s:%s.* %s:*.%s %s:*.*",
      mode, name1, name2,
      mode, name1,
      mode,        name2,
      mode);

   return str;
}

