static char rcsid[]="$Id: c_putf.c 135 2010-01-26 00:52:33Z user $";
/*
RCS-t=DDS API, put char, float

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_putf(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   void *buf,
   const float *array,
   int count)
{
   PUT_DATA("putf", FLOAT, '\0', bin_tag, sym_tag, buf, array, count)
}

