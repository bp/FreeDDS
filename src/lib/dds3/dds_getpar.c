static char rcsid[]="$Id: dds_getpar.c 16 2009-01-06 21:52:47Z user $";

/*
RCS-t=DDS application, get parameter definition and print its value.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "cdds.h"

int main(argc, argv)
   int argc;
   char **argv;
{
   char dict[PATHNAME_MAX+1];
   const char *name, *value;
   size_t size;

   setargcv(argc, argv);

   cdds_dict("cmd:", "scan");
   if(1 != cdds_scanf("cmd_args", "%" PATHNAME_MAXQ "s", dict)) return 1;
   if(0 >= cdds_scant("", &name)) return 1;
   if(cdds_dict(dict, "scan")) return 1;
   value = "";
   cdds_scanv(name, &value);
   while(isspace(*value)) value++;
   size = strlen(value);
   while(size && isspace(value[size - 1])) size--;
   fwrite(value, 1, size, stdout);
   return 0;
}
