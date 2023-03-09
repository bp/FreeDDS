static char rcsid[]="$Id: c_getcb.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, get char, blank fill

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_getcb(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const void *buf,
   char *array,
   int count)

   GET_DATA("getcb", CHAR, ' ', bin_tag, sym_tag, buf, array, count)

