static char rcsid[]="$Id: c_getf.c 135 2010-01-26 00:52:33Z user $";
/*
RCS-t=DDS API, get float

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_getf(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const void *buf,
   float *array,
   int count)
{
   GET_DATA("getf", FLOAT, '\0', bin_tag, sym_tag, buf, array, count)
}
