static char rcsid[]="$Id: c_open.c 309 2018-12-18 22:15:38Z user $";
/*
RCS-t=DDS API, open binary data, given a dictionary

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

BIN_TAG cdds_open(
   const char *open_dict,
   const char *open_format,
   const char *open_data,
   const char *open_mode)
{
   BIN_TAG bin_tag;
   DDS_BIN *bin, **tmp_bin;
   DDS_MAP **tmp_map;
   size_t bin_max;
   int i, j, ier, old_error_count;

   DDS_API_ENTRY("open")

   /************************************/
   /* Validate open_dict and open_mode */
   /************************************/

   if(cdds_dict(open_dict, "scan")) {
      dds_error = ddsError("data not opened (dictionary error)\n\tdict= %s\n",
         open_dict);
      cdds_dict("", "scan");
      DDS_API_EXIT(EOF);
   } else if(! dds_scan_first) {
      dds_error = ddsDebug("data not opened (no dictionary)\n");
      DDS_API_EXIT(EOF);
   }

   if(*open_mode != 'r' &&
      *open_mode != 'w' &&
      *open_mode != 'a' &&
      *open_mode != 'm') {
      dds_error = ddsDebug("data not opened, invalid mode (%s)\n\tdict= %s\n",
         ddsHint(open_mode), open_dict);
      cdds_dict("", "scan");
      DDS_API_EXIT(EOF);
   }

   /*********************************/
   /* Search for an unused BIN_TAG. */
   /*********************************/

   for(bin_tag = 0; bin_tag < dds_bin_max; bin_tag++)
      if(! dds_bin[bin_tag]) break;

   if(bin_tag == dds_bin_max) {
      /* dds_bin table overflow.   Double its size and continue. */
      bin_max = dds_bin_max;
      dds_bin_max *= 2;

      tmp_bin = dds_bin;
      dds_bin = (DDS_BIN**)ddsZalloc(dds_bin_max * sizeof(DDS_BIN*));
      memcpy((void*)dds_bin, (void*)tmp_bin, bin_max * sizeof(DDS_BIN*));
      ddsFree(tmp_bin);

      tmp_map = dds_map;
      dds_map = (DDS_MAP**)ddsZalloc(dds_bin_max * dds_bin_max *
         sizeof(DDS_MAP*));

      for(i = 0; i < bin_max; i++) {
         for(j = 0; j < bin_max; j++) {
            dds_map[i * dds_bin_max + j] = tmp_map[i * bin_max + j];
         }
      }
      ddsFree(tmp_map);
   }

   /***********************************************/
   /* Allocate and initialize DDS_BIN descriptor. */
   /***********************************************/

   bin = (DDS_BIN*)ddsZalloc(sizeof(DDS_BIN));

   if(dds_scan_first->alias) {
      bin->alias_name = ddsString(dds_scan_first->alias);
   } else {
      bin->alias_name = ddsString("<anonymous>");
   }

   bin->tag = bin_tag;
   bin->dict_open = ddsString(open_dict);
   bin->mode = ddsString(open_mode);
   bin->state = (DDS_BIN_STATE)(DDS_BIN_FREEZE | DDS_BIN_MSG);
   bin->block_size = -1;
   bin->bin_tag = -1;
   bin->LineTrc = 1;
   bin->ReelTrc = 1;
   bin->FileTrc = 1;

   dds_bin[bin_tag] = bin;

   if(! open_format) open_format = "";
   while(isspace(*open_format)) open_format++;

   if(! open_data) open_data = "";
   while(isspace(*open_data)) open_data++;

   /*******************************/
   /* Continue BIN initialization */
   /*******************************/

   old_error_count = dds_error_count;

   ier = ddsOpenBin(bin, open_format, open_data);

   if(ier || dds_error_count > old_error_count) {
      /* error, cleanup the mess left behind */
      ddsCloseBin(bin);
      bin_tag = -1;
   } else {
      ddsPrintInfo(bin);
   }

   cdds_dict(open_dict, "scan");

   DDS_API_EXIT(bin_tag);
}
