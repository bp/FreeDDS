static char rcsid[]="$Id: openbin.c 139 2010-02-05 21:10:52Z user $";
/*
RCS-t=DDS, open binary data, validate format and data definition

*/

/**********************************************/
/***   Copyright (C) 2005                   ***/
/***   BP America Inc., Houston, TX         ***/
/***   Jerry Ehlers, July 20, 2005          ***/
/**********************************************/

#include "dds_sys.h"

int ddsOpenBin(
   DDS_BIN *bin,
   const char *open_format,
   const char *open_data)
{
   DDS_STRING alias = DDS_NULL_STRING;
   DDS_STRING data_name = DDS_NULL_STRING;
   DDS_CUBE old_cube;
   DDS_AXIS axis;
   int n, name_max, errors=1;
   char *strhold, *str, *ptr;
   const char *value, *fmt, *fast_name, *tmp_name;
   old_cube.axis = (DDS_AXIS*)0;

   cdds_dict(bin->dict_open, "scan");

   /* Retrieve the value currently defined for "format". */
   /* It may be needed to default the format of input data */
   /* or to select a recipe when cooking up an output format. */
   if(0 < cdds_scant("format", &value)) {
      bin->old_format = ddsString(value);
   } else if(0 < cdds_scant("in", &value) &&
             0 < cdds_scant("esize", &value) &&
             0 < cdds_scant("label1", &value) &&
             0 < cdds_scant("data_format", &value)) {
      /* Appears to have been sep format */
      bin->old_format = ddsString("sep");
      cdds_dict(bin->dict_open, "print");
      cdds_printf("format","sep\n");
      cdds_dict(bin->dict_open, "scan");
   } else {
      bin->old_format = ddsString("");
   }

   if(*open_format) {
      /* application allows the format to be defined explicitly */
      ddsStrCpyCat3(&alias, DDS_SENTRY, " ", open_format);

      if(0 < cdds_scant(alias.ptr, &value) &&
         strcmp(dds_scan_name, DDS_SENTRY)) {
         /* An explicit override has been defined for the format. */
         /* i.e. the format of the binary data _must_ be this value. */
         bin->state |= DDS_BIN_FORCE_FMT;
         bin->fmt_name = ddsString(value);
         if(strcmp(open_format, "format")) {
            /* define alias between <open_format> and "format" */
            cdds_dict(bin->dict_open, "print");
            cdds_printf("$format", " %s\n", open_format);
         }
      }
   }

   cdds_dict(bin->dict_open, "scan");

   if(*open_data) {
      /* application allows data filename to be defined explicitly */
      ddsStrCpyCat3(&alias, DDS_SENTRY, " ", open_data);

      if(0 < cdds_scanv(alias.ptr, &value) &&
         strcmp(dds_scan_name, DDS_SENTRY)) {
         /* Strip leading and trailing white space from data name. */
         while(isspace(*value)) value++;
         n = strlen(value);
         while(n && isspace(value[n - 1])) n--;

         if(*value) {
            /* An explicit override has been defined for the data. */
            /* Note: setting "bin->name" here implies an override, */
            /* i.e. the name of the binary data _must_ be this value. */
            tmp_name = ddsStringN(value, n);

            /* Look for a faster alternate path */
            fast_name = ddsPathFast(tmp_name);
            bin->name = ddsString(fast_name);
            ddsFree((void*)tmp_name);

            if(strcmp(dds_scan_name, "data")) {
               /* define alias between <open_data> and "data" */
               cdds_dict(bin->dict_open, "print");
               cdds_printf("$data", " %s\n", open_data);
            }
         }
      }
   }

   if(! bin->name && *bin->mode == 'r') {
      /* An explicit override has NOT been defined for input data. */
      /* Special processing may be needed to support sep format. */
      if(bin->state & DDS_BIN_FORCE_FMT) {
         /* An explicit override has been defined for input format. */
         fmt = bin->fmt_name;
      } else {
         /* input format may be defined by the previous format. */
         fmt = bin->old_format;
      }

      if(! strcmp(fmt, "sep")) {
         /* "sep" format uses "in=" to name the binary data. */
         /* Define alias between "sep_in" ("in") and "data". */
         /* This is needed to support native sep applications. */
         cdds_dict(bin->dict_open, "print");
         cdds_printf("$data", "sep_in\n");
      }
   }
         
   cdds_dict(bin->dict_open, "scan");

   /******************************************************************/
   /* Initialize data_name, DDS_BIN_OPEN_CREATE flag, and name_max. */
   /******************************************************************/

   if(*bin->mode == 'm') {
      /* Memory mode; not a real file; i/o is not allowed. */
      if(! bin->name) {
         /* construct a fake, but informative, name for the binary data */
         ddsStrCpyCat3(&alias, "buffer(", bin->dict_open, "):");
         bin->name = ddsString(alias.ptr);
         cdds_dict(bin->dict_open, "print");
         cdds_printf("data", " %s\n", bin->name);  /* why bother ? */
      }
      ddsStrCpy(&data_name, bin->name);
      /* Slicing is disabled because its meaningless in memory mode. */
      name_max = 0;
      bin->state |= DDS_BIN_OPEN_CREATE;
   } else if(*bin->mode == 'r' || *bin->mode == 'a' || bin->name) {
      /* Initialize based upon explicit list of data names. */
      if(DDS_OK != ddsOpenNameTest(bin, &data_name, &name_max)) {
         goto cleanup;
      }
   } else if(*bin->mode == 'w') {
      /* write mode and the filenames have not been overridden. */
      bin->state |= DDS_BIN_OPEN_CREATE;
      name_max = 0;
   }

   if(bin->state & DDS_BIN_OPEN_CREATE) {
      /******************************************************/
      /* Create the binary data (don't use existing files). */
      /******************************************************/
      if(! (bin->state & DDS_BIN_FORCE_FMT)) {
         /* The format has NOT been explicitly overridden. */
         /* Default the format from "old_format" or "format". */
         cdds_dict(bin->dict_open, "scan");
         ddsStrCpyCat2(&alias, DDS_SENTRY, " old_format");

         if(0 < cdds_scant(alias.ptr, &value) &&
            strcmp(dds_scan_name, DDS_SENTRY)) {
            /* Default output format is defined by "old_format". */
            bin->fmt_name = ddsString(value);

            /* keep only the base format (first token) of the 
               old_format to avoid problems inadvertently passing 
               format fields like slice and dense.  So we no 
               longer pass old_format as an aliased format */
            cdds_dict(bin->dict_open, "print");
            cdds_printf("format", "%s\n", value);
         } else if(*bin->old_format) {
            /* Default format is defined by previous format. */
            bin->fmt_name = ddsString(bin->old_format);
         } else {
            /* Default format to _something_ and issue a warning. */
            bin->fmt_name = ddsString(DEFAULT_FORMAT);
            ddsWarn("open: defaulting output format to '%s'\n",
               bin->fmt_name);
            cdds_dict(bin->dict_open, "print");
            cdds_printf("format", " %s\n", bin->fmt_name);
         }
      }

      /* load the cube description. */
      ddsCubeScanInit(bin);
      ddsCubeScanBin(bin, &old_cube);

      if(*bin->mode != 'm' &&
         (name_max != 1 || strcmp(bin->name, "dict:"))) {
         /* Compute bin->slice_max, based upon axis and format defn. */

         if(bin->name) {
            /* Explicit override has been defined for output data name. */
            if(name_max != bin->slice_max) {
               ddsError("open: # file names= %i but slice count= %i\n\tdict= %s\n",
                  name_max, bin->slice_max, bin->dict_open);
               goto cleanup;
            }
         } else {
            /* Construct filename(s) for output data. */
            if(DDS_OK != ddsOpenNameMake(bin, &data_name)) {
               goto cleanup;
            }
            name_max = bin->slice_max;
         }
      }

      if(name_max > 1) {
         /* Open processing to create sliced data (recursive). */
         ddsOpenSlice(bin, data_name.ptr, open_format, &old_cube);
      } else {
         /* Open processing to access new data. */
/* add test in opennew to ensure data is NOT sliced. */
         ddsOpenNew(bin, data_name.ptr, &old_cube);
      }
   } else {
      /******************************************************/
      /* Use the existing binary data (don't create files). */
      /******************************************************/
      if(! data_name.ptr) {
         ddsError(
            "open, data filename is not defined (read mode)\n\talias= %s\n\tdict= %s\n",
            bin->alias_name,
            bin->dict_open);
         goto cleanup;
      }

      if(! bin->name) {
         bin->name = ddsString(data_name.ptr);
         str = strchr(bin->name, ' ');
         if (str) {
            *str = '\0';
            bin->name = (const char*)ddsRealloc((void*)bin->name, 
                                                5+strlen(bin->name));
            strcat((char*)bin->name, " ...");
         }
      }

      if(name_max > 1) {
         /* Existing data has multiple names, i.e. its sliced. */
         if(! bin->fmt_name) {
            /* The format was not explicitly overridden. */
            if(bin->old_format) {
               /* The format is provided by the master dictionary. */
               bin->fmt_name = ddsString(bin->old_format);
            } else {
               ddsError("open: format of existing sliced data is unknown\n\tdict= %s\n",
                  bin->dict_open);
               goto cleanup;
            }
         }

         /* load the cube description. */
         ddsCubeScanInit(bin);
         ddsCubeScanBin(bin, &old_cube);

         /* ensure consistant name_max and bin->slice_max */
         if(name_max != bin->slice_max) {
            ddsError("open: # file names= %i but slice count= %i\n\tdict= %s\n",
                  name_max, bin->slice_max, bin->dict_open);
            goto cleanup;
         }

         /* Open processing to access sliced data (recursive). */
         ddsOpenSlice(bin, data_name.ptr, open_format, &old_cube);
      } else {
         /* Open processing to access old data. */
         if(! (bin->state & DDS_BIN_FORCE_FMT) && bin->old_format) {
            bin->fmt_name = ddsString(bin->old_format);
         }
         
         /* bin->fmt_name may be derived from ddsAutoFmt. */
/* add test in openold to ensure data is NOT sliced. */
         ddsOpenOld(bin, data_name.ptr, &old_cube);
      }
   }

   errors = 0;
cleanup:
   if (old_cube.axis) {
      for(n=1;n<=old_cube.rank;n++) {
         axis = old_cube.axis[n];
         if(axis.name) ddsFree((void*)axis.name);
         if(axis.units) ddsFree((void*)axis.units);
         if(axis.sort_name) ddsFree((void*)axis.sort_name);
         if(axis.grid) ddsFree((void*)axis.grid);
      }
      ddsFree(old_cube.axis);
   }
   ddsStrFree(&alias);
   ddsStrFree(&data_name);

   return errors;
}
