static char rcsid[]="$Id: delayedopen.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, delayed open binary data, for convenience routines

*/

/************************************/
/***   Copyright (C) 2000         ***/
/***   BPAMOCO Corporation        ***/
/***   Jerry Ehlers 25 Aug, 2000  ***/
/************************************/

#include "dds_sys.h"

int ddsDelayedOpen(
   BIN_TAG bin_tag)
{
   const char *open_mode, *alias;
   const char *open_dict;
   const char *value, *title;
   DDS_STRING open_format = DDS_NULL_STRING;
   DDS_STRING open_data = DDS_NULL_STRING;
   BIN_TAG buf_tag, old_tag;
   DDS_BIN *bin;
   int old_error_count;

   /************************/
   /* Get open information */
   /************************/

   bin = dds_bin[bin_tag];
   if (!(bin->state & DDS_BIN_RESERVED)) return(DDS_OK);

    /* get any old_tag for openm call */
   old_tag = bin->tag;
   bin->tag = bin_tag;

   /* get information for opening this binary */
   alias = bin->alias_name;    
   open_dict = bin->dict_open;
   open_mode = bin->mode;

   /************************************/
   /* Validate open_dict and open_mode */
   /************************************/

   if (cdds_dict(open_dict, "scan")) {
     dds_error = ddsError("data not opened (dictionary error)\n\tdict= %s\n",
			  open_dict);
     cdds_dict("", "scan");
     return(EOF);
   } else if (! dds_scan_first) {
     dds_error = ddsDebug("data not opened (no dictionary)\n");
     return(EOF);
   }

   if(*open_mode != 'r' &&
      *open_mode != 'w' &&
      *open_mode != 'a' &&
      *open_mode != 'm') {
      dds_error = ddsDebug("data not opened, invalid mode (%s)\n\tdict= %s\n",
			  ddsHint(open_mode), open_dict);
      cdds_dict("", "scan");
      return(EOF);
   }

   /*******************************/
   /* Continue BIN initialization */
   /*******************************/

   old_error_count = dds_error_count;

   bin->state = (DDS_BIN_STATE)(DDS_BIN_FREEZE | DDS_BIN_MSG);

   ddsStrCpyCat2(&open_format, alias, "_format");
   ddsStrCpyCat2(&open_data, alias, "_data");

   ddsOpenBin(bin, open_format.ptr, open_data.ptr);

   ddsStrFree(&open_format);
   ddsStrFree(&open_data);

   if(dds_error_count > old_error_count) {
      ddsCloseBin(bin);
      cdds_dict(open_dict, "scan");
      return(EOF);
   }

   ddsPrintInfo(bin);

   /**************************************/
   /* Open Map from any previous old_tag */
   /* (disable mapping Samples)          */
   /**************************************/

   if (old_tag >= 0) {
      dds_map_samples = 0;
      cdds_openm(bin_tag, 0, old_tag, 0);
      dds_map_samples = 1;
   }

   /********************************/
   /* Open output binary if needed */
   /********************************/

   if (bin->bin_tag >= 0){
     buf_tag = bin_tag;
     bin_tag = bin->bin_tag;
     bin = dds_bin[bin_tag];
     open_mode = bin->mode;
     alias = bin->alias_name;    
     value = bin->value;
     title = bin->title;

     if (*open_mode == 'r') {
       bin->dict_open = cdds_mod(alias, value, title, open_dict);
     } else {
       bin->dict_open = cdds_out(alias, value, open_dict);
     }

     ddsDelayedOpen(bin_tag);

     /* map input to output */

     cdds_openm(bin_tag, 0, buf_tag, 0);
     cdds_openm(buf_tag, 0, bin_tag, 0);
   }

   return(DDS_OK);
}
