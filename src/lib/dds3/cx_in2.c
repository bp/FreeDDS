static char rcsid[]="$Id: cx_in2.c 301 2016-06-07 19:28:24Z user $";
/*
RCS-t=DDS API, convenience routine to open input data with headers & Samples
*/

/***************************************/
/***   Copyright (C) 2003            ***/
/***   BPAMOCO, Houston, TX          ***/
/***   Jerry Ehlers, February, 2000  ***/
/***************************************/

#include "dds_sys.h"

BIN_TAG cddx_in2(
   const char *alias,
   const char *value,
   const char *title)
{
   int mod;
   const char *dict, *buf_dict, *genus;
   DDS_BIN *bin;
   BIN_TAG bin_tag, buf_tag, tag;
   DDS_STRING format = DDS_NULL_STRING;
   DDS_STRING data = DDS_NULL_STRING;
   DDS_STRING buf_alias = DDS_NULL_STRING;

   DDS_API_ENTRY("xin2");

   /* check if <alias>, <alias>_data or <value> are specified */

   if (value) while(isspace(*value)) value++;
   cdds_dict("par:", "scan");
   ddsStrCpyCat4(&data, alias, " ", alias, "_data");
   if (cdds_scanf(data.ptr,"")<0) {
      if (!value || ! *value) {
         DDS_API_EXIT((BIN_TAG)-2);
      }
      if (!strcmp(value,"stdin:") && isatty(fileno(stdin))) {
         DDS_API_EXIT((BIN_TAG)-2);
      }  
   }

  /* open input */

   ddsStrCpyCat2(&format, alias, "_format");
   ddsStrCpyCat2(&data, alias, "_data");
   dict = cdds_in(alias, value, title);
   bin_tag = cdds_open(dict, format.ptr, data.ptr, "r+");
   ddsStrFree(&format);
   ddsStrFree(&data);
   if (bin_tag < 0) {
      cdds_prterr("Unable to open input (%s) file\n", alias);
      cdds_dict(dict, "close");
      DDS_API_EXIT(bin_tag)
   }
   
   /* open buffer */

   ddsStrCpyCat2(&buf_alias, alias, "_buf");
   ddsStrCpyCat2(&format, buf_alias.ptr, "_format");
   ddsStrCpyCat2(&data, buf_alias.ptr, "_data");

   cdds_dict("override:", "scan");
   mod = cdds_scanf("MOD_FIELD","\0");
   cdds_dict("override:", "print");
   if (mod < 0) {
      cdds_printf("MOD_FIELD", "-+ SAMPLE_TYPE Samples[axis_size(1)];\n");
   }

   tag = cdds_member(bin_tag, 0, "Samples");
   if (tag >= 0) {
      genus = cdds_genus(bin_tag, tag);

      cdds_dict("override:", "print");
      if (!strcmp(genus,"complex")) {
         cdds_printf("fmt:SAMPLE_TYPE", "typedef complex SAMPLE_TYPE;\n");
      } else {
         cdds_printf("fmt:SAMPLE_TYPE", "typedef float SAMPLE_TYPE;\n");
      }
   }

   buf_dict = cdds_out(buf_alias.ptr, "", dict);
   cdds_printf("$old_format", "format\n");
   cdds_printf(format.ptr, "asp\n");
   buf_tag = cdds_open(buf_dict, format.ptr, data.ptr, "m");

   cdds_dict("override:", "print");
   cdds_printf("$MOD_FIELD", "\n");

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
