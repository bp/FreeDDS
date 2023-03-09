static char rcsid[]="$Id: stringn.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, malloc and initialize simple string

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * ddsStringN(
   const char *old_str,
   size_t old_size)
{
   char *new_str;

   new_str = (char*)ddsMalloc(old_size + 1);
   memcpy(new_str, old_str, old_size);
   new_str[old_size] = '\0';

   return new_str;
}
