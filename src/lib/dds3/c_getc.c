static char rcsid[]="$Id: c_getc.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, get complex

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_getc(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const void *buf,
   float *array,
   int count)

   GET_DATA("getc", CPX, '\0', bin_tag, sym_tag, buf, array, count)

