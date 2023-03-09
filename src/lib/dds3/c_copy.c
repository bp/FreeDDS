static char rcsid[]="$Id: c_copy.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, copy one scan definition to print dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int cdds_copy(
   const char *alias)
{
   int status;
   const char *value;

   DDS_API_ENTRY("copy")

   if(dds_print_first) {
      if(dds_scan_dict)
         dds_scan_token.next = dds_scan_dict->buf.ptr + dds_scan_value_bias;
      status = cdds_scanv(alias, &value);
      if(status >= 0)
         status = cdds_printv(dds_scan_name, value);
   } else {
      status = EOF;
   }

   DDS_API_EXIT(status)
}
