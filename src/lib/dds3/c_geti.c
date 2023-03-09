static char rcsid[]="$Id: c_geti.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, get integer

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_geti(
   BIN_TAG bin_tag,
   SYM_TAG sym_tag,
   const void *buf,
   int *array,
   int count)

   GET_DATA("geti", INT, '\0', bin_tag, sym_tag, buf, array, count)

