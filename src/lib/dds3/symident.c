static char rcsid[]="$Id: symident.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, validate syntax of identifier for symbols

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

const char * ddsSymIdent(
   char *str)
{
   DDS_BOOLEAN error = DDS_FALSE;
   char *name = str, test;

   if(! str || ! *str) {
      error = DDS_TRUE;
      name = "<identifer>";
   } else {
      for(test = *str; test; test = *++str) {
         if(test == '=' || test == '\\' || ! isgraph(test)) {
            error = DDS_TRUE;
            *str = '~';
         }
      }
   }

   if(error) {
      ddsYYError("ddsSymIdent: bad identifier name= '%s'\n", name);
   }

   return name;
}
