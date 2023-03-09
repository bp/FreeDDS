static char rcsid[]="$Id: opennamemake.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, open binary data, make filenames for output data.

*/

/**********************************************/
/***   Copyright (C) 2005                   ***/
/***   BP America Inc., Houston, TX         ***/
/***   Jerry Ehlers, July 20, 2005          ***/
/**********************************************/

#include "dds_sys.h"

int ddsOpenNameMake(
   DDS_BIN *bin,
   DDS_STRING *data_name)
{
   DDS_STRING stem_name = DDS_NULL_STRING;
   DDS_DICT *out_dict;
   int sliced;
   int slice_max_log10;
   int slice_num, slice_mod;
   char slice_name[32], *format, *str;
   const char *value, *name_gap;
   const char *suffix_suffix, *fast_name;

   cdds_dict(bin->dict_open, "scan");

   /* Remember the output dictionary. */
   out_dict = dds_scan_first;

   /* Create default filename(s) for output data. */
   if(out_dict->media &&
      (out_dict->name[strlen(out_dict->name) - 1] == ':' ||
         ddsFstatGenus(gfileno(out_dict->media->fp)) == DDS_MEDIA_FIFO)) {
      /***
          25 Mar 1998, RLS, good ? bad ? or ugly ?
          the binary data is attached to the output dictionary
          if the end user specifies a dictionary name ending with ":"
      ***/
      /* Attach binary data to dictionary. */
      ddsStrCpy(data_name, "dict:");
      /* Multiple slices are ignored when data is attached. */
      bin->slice_max = 1;
      /* Remove any slice information from format */
      cdds_scanv("format", &value);
      format = (char*)malloc(1+strlen(value));
      format[0] = '\0';
      sliced = 0;
      cdds_scanf("format", "");
      while(cdds_scant("", &value) > 0) {
         if (strncmp(value, "slice.", 6)) {
            strcat(format, value);
            strcat(format, " ");
         } else {
            sliced = 1;
         }
      }
      if (sliced) {
         cdds_dict(bin->dict_open, "print");
         cdds_printf("format", "%s\n", format);
      }
      free(format);
   } else {
      /* Construct filename(s) from 3 components, i.e. */
      /*    <stem_name> + <data_suffix> + <suffix_suffix> */
/*** 28 July 1998 RLS, bad idea
      cdds_dict(dds_base_dict, "scan");
***/

      if(0 < cdds_scant(dds_data_path.ptr, &value)) {
         ddsStrCpy(&stem_name, value);
      } else {
         ddsStrCpy(&stem_name, dds_cmd_cwd);
      }
#if defined(_WIN32) || defined(MINGW)
      /* For Windows, path could contain forward and backward slash. */
      if (stem_name.ptr[strlen(stem_name.ptr) - 1] != '/') {
         ddsStrCat(&stem_name, "\\");
      } else if (stem_name.ptr[strlen(stem_name.ptr) - 1] != '\\') {
         ddsStrCat(&stem_name, "\\");
      }
      value = strrchr(out_dict->name, '/');
      if (value) {
         /* Dictionary name contains a path prefix. */
         ddsStrCat(&stem_name, ++value);
      } else {
         value = strrchr(out_dict->name, '\\');
         if (value) {
            /* Dictionary name contains a path prefix. */
            ddsStrCat(&stem_name, ++value);
         } else {
            /* Simple dictionary name, without any path prefix. */
            value = out_dict->name;
            ddsStrCat(&stem_name, out_dict->name);
         }
      }

      /* Look for a faster alternate path */
      fast_name = ddsPathFast(stem_name.ptr);
      ddsStrCpy(&stem_name, fast_name);

      if (!strncmp(value, "tmp", 3) &&
         value[strlen(value) - 1] == ':') {
         suffix_suffix = "";  /* Windows can not have ":" for filename */
#else
      if(stem_name.ptr[strlen(stem_name.ptr) - 1] != '/') {
         ddsStrCat(&stem_name, "/");
      }

      value = strrchr(out_dict->name, '/');

      if(value) {
         /* Dictionary name contains a path prefix. */
         ddsStrCat(&stem_name, ++value);
      } else {
         /* Simple dictionary name, without any path prefix. */
         value = out_dict->name;
         ddsStrCat(&stem_name, out_dict->name);
      }

      /* Look for a faster alternate path */
      fast_name = ddsPathFast(stem_name.ptr);
      ddsStrCpy(&stem_name, fast_name);

      if(! strncmp(value, "tmp", 3) &&
         value[strlen(value) - 1] == ':') {
         /* Output dictionary is tmp, trailing ":" is needed. */
         suffix_suffix = ":";

#endif

         bin->state |= DDS_BIN_TMP;
      } else {
         suffix_suffix = "";
      }

      if(bin->slice_max == 1) {
         /* Data is NOT sliced, create one name. */
         if(0 <= cdds_scant(dds_data_suffix.ptr, &value)) {
            /* Explicit suffix for filename. */
            ddsStrCpyCat3(data_name, stem_name.ptr,
               value, suffix_suffix);
         } else {
            /* Default suffix for filename. Change to "_@" */
            /* ddsStrCpyCat4(data_name, stem_name.ptr,
               ".", bin->fmt_name, suffix_suffix); */
            ddsStrCpyCat3(data_name, stem_name.ptr,
               "_@", suffix_suffix);
         }
      } else {
         /* Data IS sliced, create a name for each slice. */
         /* Maximum number of digits in slice number. */
         slice_max_log10 = 1 + log10((double)bin->slice_max);

         /* Maximum number of filenames per line in dictionary. */
         slice_mod = DDS_SLICE_LINE /
            (3 + slice_max_log10 + strlen(stem_name.ptr));
         if(! slice_mod) slice_mod = 1;

         for(slice_num = 1; slice_num <= bin->slice_max; slice_num++) {
            /* filenames sort nicer when leading zeros are padded */
            sprintf(slice_name, ".%0*i", slice_max_log10, slice_num);
            if(slice_num == 1) {
               /* special case for first filename */
               name_gap = "";
            } else if((slice_num - 1) % slice_mod) {
               /* append filename to current line */
               name_gap = "  ";
            } else {
               /* start another line for this filename */
               name_gap = "\n   ";
            }

            ddsStrCat4(data_name, name_gap,
               stem_name.ptr, slice_name, suffix_suffix);
         }
      }
   }

   cdds_dict(bin->dict_open, "print");
   cdds_printf("data", " %s\n", data_name->ptr);
   if(! strcmp(bin->fmt_name, "sep")) {
      cdds_printf("in", "\"%s\"\n", data_name->ptr);
   }

   bin->name = ddsString(data_name->ptr);
   str = strchr(bin->name, ' ');
   if (str) {
      *str = '\0';
      bin->name = (const char*)ddsRealloc((void*)bin->name,
                                          5+strlen(bin->name));
      strcat((char*)bin->name, " ...");
   }
   ddsStrFree(&stem_name);

   return DDS_OK;
}
