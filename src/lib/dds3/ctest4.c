static char rcsid[]="$Id: ctest4.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test parameter dictionary simplest functionality.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>

#include "dds_sys.h"

int main(
   int argc,
   char **argv)
{
   const char *value;
   int rc;

   setargcv(argc, argv);
 
   cdds_init();
   rc = cdds_dict("par:", "scan");
   value = "undefined";
   rc = cdds_scanv("hello", &value);
   printf("hello=%s\n", value);
   value = "undefined";
   rc = cdds_scanv("bello", &value);
   printf("bello=%s\n", value);

   return 0;
}
