static char rcsid[]="$Id: c_putl.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, put char, long

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Jerry Ehlers, 16 July 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_putl(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   void *buf,
   const long *array,
   int count)

   PUT_DATA("putl", LONG, '\0', bin_tag, sym_tag, buf, array, count)

