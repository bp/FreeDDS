static char rcsid[]="$Id: aliasname3.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, create defn alias names

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_STRING * ddsAliasName3(
   DDS_STRING *str,
   const char *mode,
   const char *name1,
   const char *name2,
   const char *name3)
{
   size_t str_size;

   /*******************************************/
   /*  The syntax of the defn alias is...     */
   /*     "mode:name1:name2.name3"            */
   /*        +  " mode:name1:name2.*"         */
   /*        +  " mode:name1:*.name3"         */
   /*        +  " mode:name1:*.*"             */
   /*        +  " mode:*:name2.name3"         */
   /*        +  " mode:*:name2.*"             */
   /*        +  " mode:*:*.name3"             */
   /*        +  " mode:*:*.*"                 */
   /*                                         */
   /*  where "mode" is "map" or "fmt".        */
   /*  where "name1" and "name2" are either   */
   /*     dictionary aliases ("in", "out"...) */
   /*     or format names ("usp", "segy"...)  */
   /*******************************************/

   str_size = 68 + 8 * strlen(mode)  + 4 * (strlen(name1) + strlen(name2) + strlen(name3));

   ddsStrCpyMin(str, "", str_size);

   sprintf(str->ptr, "%s:%s:%s.%s %s:%s:%s.* %s:%s:*.%s %s:%s:*.* %s:*:%s.%s %s:*:%s.* %s:*:*.%s %s:*:*.*",
      mode, name1, name2, name3,
      mode, name1, name2,
      mode, name1,        name3,
      mode, name1,
      mode,        name2, name3,
      mode,        name2,
      mode,               name3,
      mode);

   return str;
}
