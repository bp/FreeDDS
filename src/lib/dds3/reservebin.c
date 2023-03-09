static char rcsid[]="$Id: reservebin.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, reserve a bin tag for later opening by 
convenience routines.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/
/* Reserve a bin tag for later delayed        */
/* output opening (ddsDelayedOpen) and        */
/*    1) save bin_tag of previous bin         */
/* or 2) save dictionary name for this bin    */ 
/**********************************************/

#include "dds_sys.h"

BIN_TAG ddsReserveBin(
   const char *alias,
   const char *value,
   const char *title,
   const char *open_mode)
{
   int i, j;
   BIN_TAG bin_tag;
   DDS_BIN *bin, **tmp_bin;
   DDS_MAP **tmp_map;
   size_t bin_max;

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

      for(i=0;i<bin_max;i++) {
         for(j=0;j<bin_max;j++) {
            dds_map[i * dds_bin_max + j] = tmp_map[i * bin_max + j];
         }
      }
      ddsFree(tmp_map);
   }

   /***********************************************/
   /* Allocate and initialize DDS_BIN descriptor. */
   /***********************************************/

   bin = (DDS_BIN*)ddsZalloc(sizeof(DDS_BIN));

   bin->alias_name = ddsString(alias);
   bin->tag = -1;
   bin->mode = ddsString(open_mode);
   bin->state = (DDS_BIN_STATE)(DDS_BIN_RESERVED);
   bin->block_size = -1;
   bin->bin_tag = -1;
   bin->LineTrc = 1;
   bin->ReelTrc = 1;
   bin->FileTrc = 1;

   dds_bin[bin_tag] = bin;

   /***************************/
   /* Save for later opening. */
   /***************************/
   
   bin->value = ddsString(value);
   bin->title = ddsString(title);

   return(bin_tag);
}
