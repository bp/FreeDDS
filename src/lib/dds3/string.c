static char rcsid[]="$Id: string.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, malloc and initialize simple string

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * ddsString(
   const char *old_str)
{
   char *new_str;

   new_str = (char*)ddsMalloc(strlen(old_str) + 1);
   strcpy(new_str, old_str);

   return new_str;
}
