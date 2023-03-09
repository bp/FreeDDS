static char rcsid[]="$Id: testcomp2.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test compiler functionality.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "parser.tab.h"

int main(
   int argc,
   char **argv)
{
   const char *in_dict;
   BIN_TAG in_bin;
   char buf[40000];
   int rc;

   setargcv(argc, argv);
 
   cdds_init();
   dds_state |= DDS_VERBOSE;

   in_dict = cdds_in("in", "stdin:", "testcomp2");
   cdds_dict(in_dict, "print");
   cdds_printf("format", " usp\n");
   cdds_printf("data", " classic.usp\n");
   in_bin = cdds_open(in_dict, "in_format", "in_data", "r");

   ddsDumpSymbol();

   return 0;
}
