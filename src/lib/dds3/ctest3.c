static char rcsid[]="$Id: ctest3.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS main, test preprocessor

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int main(
   int argc,
   char **argv)
{
   const char *value;
   int rc;

   setargcv(argc, argv);
 
   #if (DDS_INTEGER2 == HOST_SHORT_DDS && \
        DDS_INTEGER8 == HOST_LONG_DDS)
      printf("yes 1\n");
   #elif (DDS_INTEGER2 == HOST_SHORT_DDS && \
          DDS_INTEGER8 == HOST_LLONG_DDS)
      printf("yes 2\n");
   #else
      printf("no 1\n");
   #endif

   return 0;
}
