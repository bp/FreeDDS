static char rcsid[]="$Id: c_getd.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, get double

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include <string.h>
#include "dds_sys.h"

int cdds_getd(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const void *buf,
   double *array,
   int count)

   GET_DATA("getd", DOUBLE, '\0', bin_tag, sym_tag, buf, array, count)

