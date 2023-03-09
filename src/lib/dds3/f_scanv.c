static char rcsid[]="$Id: f_scanv.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS API, select a scan definition and retrieve literal value.

FEATURE: Trailing blanks are padded as needed to Fortran character values.
The actual length of the defined literal value is returned as an integer.
It may be more or less than the Fortran character string length.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

#define ALIAS_NOM_LEN 4 * DEFNNAME_MAX


HOST_FTN_INT
HOST_FTN_FUNC(fdds_scanv, FDDS_SCANV)(
   HOST_FTN_STR alias_ftn,
   HOST_FTN_STR value_ftn,
   HOST_FTN_LEN alias_len,
   HOST_FTN_LEN value_len)
{
   int status = DDS_OK;
   static int ivalue = 0;
   static int nvalue = 0;
   static const char *value;
   static DDS_STRING alias_str = NULL_STRING;

   DDS_API_INIT;

   FTN_STR_TO_STR(alias_str, alias_ftn, alias_len);

   if (!alias_str.ptr[0] && ivalue < nvalue) {
      STR_TO_FTN_STR(value_ftn, value_len, &value[ivalue]);
      status = nvalue - ivalue;
      if (status > value_len) status = value_len;
      ivalue += status;
   } else {
      ivalue = 0;
      nvalue = 0;
      status = cdds_scanv(alias_str.ptr, &value);
      if (status != EOF) {
         STR_TO_FTN_STR(value_ftn, value_len, value);
         nvalue = status;
         if (status > value_len) status = value_len;
         ivalue = status;
      }
   } 

   return status;
}
