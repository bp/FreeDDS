static char rcsid[]="$Id: ctest1.c 16 2009-01-06 21:52:47Z user $";
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
   fprintf(stderr,"init\n");
   dds_state &= (DDS_STATE) ~ DDS_ERROR_ABORT;
   rc = cdds_dict("stdin:", "scan");
   fprintf(stderr,"dict(stdin:,scan)=%d\n",rc);
   rc = cdds_scanv("hello", &value);
   fprintf(stderr,"scanv(hello)=%d\n",rc);
   rc = cdds_dict("stdout:", "print");
   fprintf(stderr,"dict(stdout:,print)=%d\n",rc);
   rc = cdds_printv("wallo", " hollow\n");
   fprintf(stderr,"printv(wallo)=%d\n",rc);
   rc = cdds_dict("stdout:", "reset");
   fprintf(stderr,"dict(stdout:,reset)=%d\n",rc);
   rc = cdds_dict("missing", "scan");
   fprintf(stderr,"dict(missing,scan)=%d\n",rc);

   return 0;
}
