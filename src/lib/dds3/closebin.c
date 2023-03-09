static char rcsid[]="$Id: closebin.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS API, close binary data

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsCloseBin(
   DDS_BIN *bin)
{
   DDS_MEDIA *media, *media2;
   DDS_AXIS *axis;
   DDS_MAP *map_ptr, *map_ptr_next;
   BIN_TAG bin_tag, sibling_bin_tag;
   DDS_BIN *slice_bin, *sibling_bin;
   DDS_VALUE *value, **global, **global_stop;
   int n, rank, rank_max;
   int status = DDS_OK;

   if (!bin || bin->state & DDS_BIN_RESERVED) return status;

   errno = 0;
   bin->state |= DDS_BIN_CLOSED;
   dds_compile = NULL;
   bin_tag = bin->tag;
   rank_max = bin->cube.rank;

   cdds_dict(bin->dict_open, "close");

   if(bin->media) {
      ddsPrintCloseInfo(bin);
      ddsCloseFlush(bin);

      media = bin->media;

      if(media->state & DDS_MEDIA_SWAP) {
         /*************************/
         /* swap media processing */
         /*************************/
         if(media->state & DDS_MEDIA_UNLOAD) {
            /* unload explicitly requested upon close, */
            /* or this is NOT the first tape in the list. */
            ddsMediaUnload(media);
         }
      }

      /*****************/
      /* Release media */
      /*****************/

      while(media->prev) media = media->prev;

      for(media2 = media; media2; media2 = media2->next) {
         media2->bin = NULL;
      }

      if(! media->dict) {
         if(ddsMediaDelete(media)) status = DDS_BAD;
      }

   } 

   /**************************************/
   /* Master binary for multiple slices. */
   /**************************************/
   if(bin->slice) {
      for(n = 0; n < bin->slice_max; n++) {
         slice_bin = bin->slice[n];
         if(slice_bin) {
            cdds_close(slice_bin->tag);
         }
      }
   }

   /**********************************/
   /* Free format specific resources */
   /**********************************/
   switch(bin->fmt) {
   case DDS_FMT_GENERIC:
      if(bin->std.gen.table) ddsFree(bin->std.gen.table);
      break;
   case DDS_FMT_USP:
      if(bin->std.usp.line_buf) ddsFree(bin->std.usp.line_buf);
      break;
   case DDS_FMT_SEGY:
   case DDS_FMT_SEGY1:
   case DDS_FMT_SU:
      if(bin->std.segy.card_buf) ddsFree(bin->std.segy.card_buf);
      if(bin->std.segy.binary_buf) ddsFree(bin->std.segy.binary_buf);
      break;
   }

   /**************************/
   /* Free generic resources */
   /**************************/

   for(sibling_bin_tag = 0;
      sibling_bin_tag < dds_bin_max;
      sibling_bin_tag++) {
      sibling_bin = dds_bin[sibling_bin_tag];
      if(! sibling_bin) continue; /* not open */

      /* terminate maps into other bins from this bin_tag */
      for(map_ptr = MAP_PTR(sibling_bin_tag, bin_tag);
         map_ptr;
         map_ptr = map_ptr_next) {
         map_ptr_next = map_ptr->next;
         ddsFree(map_ptr);
      }
      MAP_PTR(sibling_bin_tag, bin_tag) = NULL;

      /* terminate maps into this bin tag from other bins */
      for(map_ptr = MAP_PTR(bin_tag, sibling_bin_tag);
         map_ptr;
         map_ptr = map_ptr_next) {
         map_ptr_next = map_ptr->next;
         ddsFree(map_ptr);
      }
      MAP_PTR(bin_tag, sibling_bin_tag) = NULL;
   }

   global_stop = (DDS_VALUE**)(bin->relocate.ptr + bin->relocate.top);

   /* release symbol relocation stacks */
   for(global = (DDS_VALUE**)bin->relocate.ptr;
      global != global_stop;
      global++) {
      value = *global;
      if(! value->reloc) continue; /* something is rotten */
      ddsStackTerm(value->reloc);
      ddsFree(value->reloc);
   }

   ddsTableTerm(&bin->symbol);

   if(bin->trace_buf) ddsFree(bin->trace_buf);
   if(bin->dead_trace) ddsFree(bin->dead_trace);
   if(bin->diff1_buf) ddsFree(bin->diff1_buf);
   if(bin->diff2_buf) ddsFree(bin->diff2_buf);

   ddsStackTerm(&bin->sym_tag);
   ddsStackTerm(&bin->relocate);

   for(rank = 1; rank <= rank_max; rank++) {
      axis = bin->cube.axis + rank;
      if(axis->name) ddsFree((void*)axis->name);
      if(axis->units) ddsFree((void*)axis->units);
      if(axis->sort_name) ddsFree((void*)axis->sort_name);
      if(axis->grid) ddsFree((void*)axis->grid);
   }
   ddsFree(bin->cube.axis);

   if(bin->name) ddsFree((void*)bin->name);
   if(bin->mode) ddsFree((void*)bin->mode);
   if(bin->old_format) ddsFree((void*)bin->old_format);
   if(bin->alias_name) ddsFree((void*)bin->alias_name);
   if(bin->fmt_name) ddsFree((void*)bin->fmt_name);
   if(bin->dict_open) ddsFree((void*)bin->dict_open);
   if(bin->dict_fmt) ddsFree((void*)bin->dict_fmt);
   if(bin->dict_map) ddsFree((void*)bin->dict_map);

   if(bin->value) ddsFree((void*)bin->value);
   if(bin->title) ddsFree((void*)bin->title);

   if(bin->dump_item) ddsFree((void*)bin->dump_item);

   dds_bin[bin->tag] = NULL;

   ddsFree((void*)bin);

   return status;
}
