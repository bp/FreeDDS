static char rcsid[]="$Id: f_sscanf.c 117 2009-09-29 19:26:22Z user $";
/*
RCS-t=DDS API, scan a string for formatted values.

*/

/******************************************/
/***   Copyright (C) 2009               ***/
/***   BP America Inc., Houston, TX     ***/
/***   Jerry Ehlers, June 2009          ***/
/******************************************/

#include "dds_sys.h"

#define FMT_NOM_LEN 2 * DDS_KB

HOST_FTN_INT
HOST_FTN_FUNC(fdds_sscanf, FDDS_SSCANF)(
   HOST_FTN_STR str_ftn,
   HOST_FTN_STR fmt_ftn,
   ...)
{
   static DDS_STRING str_str = NULL_STRING;
   static DDS_STRING fmt_str = NULL_STRING;

   const char *str;
   int status = DDS_OK;
   size_t save_size;
   const DDS_DEFN *defn;
   DDS_DICT *dict;
   va_list ap, ap_len;

   HOST_FTN_LEN str_len, fmt_len;

   DDS_API_ENTRY("sscanf")

   FTN_FMT_TO_STR(fmt_str, fmt_ftn);

   va_start(ap, fmt_ftn);
   /* NOTE: explaination of the following insane kludge...
      The length information for Fortran character arguments
      appears in the argument list following the normal pointers.
      "ap_len" contains an independent copy of va_list info.
      It must be walked pasted the normal pointers,
      using the format '%' conversion specifications.
      The format and argument conversion can then proceed,
      using the original "ap" (pointers to actual args) and
      "ap_len" (length of Fortran character args).
   */

   /* Walk "ap_len" past the pointers to actual args */
   memcpy((void*)&ap_len, (void*)&ap, sizeof(va_list));
   status = ddsScanfF(&dds_scan_token.next, &ap_len, NULL, fmt_str.ptr,
      "<fake>", "<fake>");
   if(status == EOF) {
      va_end(ap);
      DDS_API_EXIT(EOF);
   }

   /* length of "str" character argument */
   str_len = va_arg(ap_len, HOST_FTN_LEN);

   /* length of "fmt" character argument */
   fmt_len = va_arg(ap_len, HOST_FTN_LEN);
   if (fmt_len < strlen(fmt_str.ptr)) {
      ddsError("fdds_sscanf: definition format (%s)\n\tnull termination length=%d > length=%d\n",
               fmt_str.ptr, strlen(fmt_str.ptr), fmt_len);
      va_end(ap);
      DDS_API_EXIT(EOF);
   }

   FTN_STR_TO_STR(str_str, str_ftn, str_len);

   str = str_str.ptr;

   status = ddsScanfF(&str, &ap, &ap_len, fmt_str.ptr, 
      "<str:>", (const char*)str);

   va_end(ap);

   DDS_API_EXIT(status);
}
