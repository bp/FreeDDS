static char rcsid[]="$Id: reformname.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, retrieve and default cube description

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define SORT_ALIAS_MAX 1024

const char * ddsReformName(
   int rank,
   const char *format,
   const char *axis_name,
   const char *old_name,
   const char *old_format)
{
   const char *new_name;
   char sort_alias[SORT_ALIAS_MAX];

   sort_alias[SORT_ALIAS_MAX - 1] = '\0';

   /* reform sort/sync field names */
   sprintf(sort_alias,
      "sort:%i  sort:%s  sort:%s.%i:%s  sort:*.%i:%s  sort:%s.%s:%s  sort:%s.*:%s  sort:*.%s:%s  sort:*.*:%s  sort:*.*:*",
      rank,
      axis_name,
      old_format, rank, format,
      rank, format,
      old_format, old_name, format,
      old_format, format,
      old_name, format,
      format);

   if(sort_alias[SORT_ALIAS_MAX - 1])
      ddsPanic("ddsCubeScan:ReformName: sort_alias overflow\n");

   new_name = old_name;
   cdds_scant(sort_alias, &new_name);

   return new_name;
}
