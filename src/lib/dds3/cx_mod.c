static char rcsid[]="$Id: cx_mod.c 126 2009-11-11 21:15:48Z user $";
/*
RCS-t=DDS API, convenience routine to open existing data with Samples 
only 
*/

/***************************************/
/***   Copyright (C) 2006            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, June, 2006      ***/
/***************************************/

#include "dds_sys.h"

BIN_TAG cddx_mod(
   const char *alias,
   const char *value,
   const char *title,
   BIN_TAG old_tag)
{
   int ier;
   const char *dict, *buf_dict, *genus;
   DDS_BIN *bin;
   BIN_TAG bin_tag, buf_tag, tag;
   DDS_STRING format = DDS_NULL_STRING;
   DDS_STRING data = DDS_NULL_STRING;
   DDS_STRING buf_alias = DDS_NULL_STRING;
   DDS_STRING fmt = DDS_NULL_STRING;

   DDS_API_ENTRY("xmod")

   /* check if <alias>, <alias>_data or <value> are specified */

      if (value) while(isspace(*value)) value++;
   cdds_dict("par:", "scan");
   ddsStrCpyCat4(&data, alias, " ", alias, "_data");
   if (cdds_scanf(data.ptr,"")<0 && (!value || ! *value)) {
      DDS_API_EXIT((BIN_TAG)-2)
   }

   /* open input */

   ddsStrCpyCat2(&format, alias, "_format");
   ddsStrCpyCat2(&data, alias, "_data");
   if (old_tag >= 0 && old_tag < dds_bin_max) {
      dict = dds_bin[old_tag]->dict_open;
   } else {
      dict = (char*)0;
   }
   dict = cdds_mod(alias, value, title, dict);
   bin_tag = cdds_open(dict, format.ptr, data.ptr, "r+");
   if (bin_tag < 0) {
      bin_tag = cdds_open(dict, format.ptr, data.ptr, "w+");
   }
   ddsStrFree(&format);
   ddsStrFree(&data);
   if (bin_tag < 0) {
      cdds_prterr("Unable to open mod file (%s)\n", alias);
      cdds_dict(dict, "close");
      DDS_API_EXIT(bin_tag)
   }
   
   /* open buffer */

   tag = cdds_member(bin_tag, 0, "Samples");
   genus = cdds_genus(bin_tag, tag);

   ddsStrCpyCat2(&buf_alias, alias, "_buf");
   ddsStrCpyCat2(&format, buf_alias.ptr, "_format");
   ddsStrCpyCat2(&data, buf_alias.ptr, "_data");
   ddsStrCpyCat3(&fmt, "fmt:*:", buf_alias.ptr, ".ASP_TRACE");

   cdds_dict("override:", "print");
   cdds_printf(fmt.ptr, "typedef struct {\n");
   cdds_printf("", "   + SAMPLE_TYPE Samples[axis_size(1)];\n");
   cdds_printf("", "   } ASP_TRACE;\n");
   
   cdds_dict("override:", "scan");
   ier = cdds_scanf("fmt:SAMPLE_TYPE","\0");
   cdds_dict("override:", "print");
   if (ier < 0) {
      if (!strcmp(genus,"complex")) {
         cdds_printf("fmt:SAMPLE_TYPE", "   typedef complex SAMPLE_TYPE;\n");
      } else {
         cdds_printf("fmt:SAMPLE_TYPE", "   typedef float SAMPLE_TYPE;\n");
      }
   }

   buf_dict = cdds_out(buf_alias.ptr, "", dict);
   cdds_printf("$old_format", "format\n");
   cdds_printf(format.ptr, "asp\n");
   buf_tag = cdds_open(buf_dict, format.ptr, data.ptr, "m");
   if (buf_tag < 0) {
      cdds_prterr("Unable to open internal (%s) buffer\n", alias);
      cdds_dict("override:", "reset");
      cdds_dict(buf_dict, "close");
      cdds_dict(dict, "close");
      cdds_close(bin_tag);
      DDS_API_EXIT(buf_tag)
   }

   /* map input to buffer */

   cdds_openm(buf_tag,0,bin_tag,0);
   cdds_openm(bin_tag,0,buf_tag,0);
   cdds_dict("override:", "reset");

   /* save info */

   bin = dds_bin[buf_tag];
   bin->bin_tag = bin_tag;

   DDS_API_EXIT(buf_tag)
}
