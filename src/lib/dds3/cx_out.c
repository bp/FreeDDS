static char rcsid[]="$Id: cx_out.c 230 2011-07-05 23:53:12Z user $";
/*
  RCS-t=DDS API, convenience routine to open output data

  NOTE: override: must NOT be cleared until all the input & output 
  binaries have actually been opened.  The SAMPLE_TYPE can be 
  overridden by the application by rewriting a newer defintion in the 
  override: dictionary;  eg. the delayed open can actually be forced 
  by seeking to the beginning.

*/

/***************************************/
/***   Copyright (C) 2000            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"

BIN_TAG cddx_out(
   const char *alias,
   const char *value,
   const char *title,
   BIN_TAG old_tag)
{
   const char *old_dict, *buf_dict;
   int ier;
   DDS_BIN *buf_bin;
   BIN_TAG bin_tag, buf_tag;
   DDS_STRING buf_alias = DDS_NULL_STRING;
   DDS_STRING tmp = DDS_NULL_STRING;

   DDS_API_ENTRY("xout");

   /* check if <alias>, <alias>_data or <value> are specified */

   if (value) while(isspace(*value)) value++;
   cdds_dict("par:", "scan");
   ddsStrCpyCat4(&tmp, alias, " ", alias, "_data");
   if (cdds_scanf(tmp.ptr,"")<0 && (!value || ! *value)) {
      DDS_API_EXIT((BIN_TAG)-2);
   }

   /* reserve bin tags */

   bin_tag = ddsReserveBin(alias, value, title, "w+");
   if (bin_tag < 0) {
      cdds_prterr("Unable to reserve output (%s) bin\n", alias);
      DDS_API_EXIT(bin_tag);
   }

   ddsStrCpyCat2(&buf_alias, alias, "_buf");
   buf_tag = ddsReserveBin(buf_alias.ptr, "", title, "m");
   if (buf_tag < 0) {
      cdds_prterr("Unable to reserve internal (%s) bin\n", buf_alias.ptr);
      ddsStrFree(&buf_alias);
      DDS_API_EXIT(buf_tag);
   }

   /* setup buffer dictionary from previous dictionary */
   if (old_tag >= 0 && old_tag < dds_bin_max) {
      old_dict = dds_bin[old_tag]->dict_open;

      /* setup buffer dictionary from scratch */
      buf_dict = cdds_out(buf_alias.ptr, "", old_dict);

   } else {
      old_dict = cdds_in("", "", title);

      buf_dict = cdds_out(buf_alias.ptr, "", old_dict);
      cdds_dict(buf_dict,"scan");
      ier = cdds_scanf("old_format","");
      cdds_dict(buf_dict,"print");
      if (ier < 0) cdds_printf("old_format", "asp\n");
      cdds_printf("fmt:ASP_TRACE", "typedef struct {\n");
      cdds_printf("", "   SAMPLE_TYPE Samples[axis_size(1)];\n");
      cdds_printf("", "   } ASP_TRACE;\n");
      cdds_printf("fmt:SAMPLE_TYPE", "  typedef float SAMPLE_TYPE;\n");
   } 
   ddsStrCpyCat2(&tmp, buf_alias.ptr, "_format");
   cdds_printf(tmp.ptr, "asp\n");

   /* clear local info */

   ddsStrFree(&tmp);
   ddsStrFree(&buf_alias);
   if (old_tag < 0 || old_tag >= dds_bin_max) {
      cdds_dict(old_dict, "close");
      cdds_free((void*)old_dict);
   }

   /* save info for delayed opening */

   buf_bin = dds_bin[buf_tag];
   buf_bin->dict_open = buf_dict;
   buf_bin->tag = old_tag;
   buf_bin->bin_tag = bin_tag;

   DDS_API_EXIT(buf_tag);
}
