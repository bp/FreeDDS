static char rcsid[]="$Id: opennext.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, open next file/media on device

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"
#include "dds_magtape.h"

int ddsOpenNext(
   DDS_BIN *bin,
   const char *mode)
{
   DDS_MEDIA *chain, *media = bin->media;
   int n_label;
   char scratch[128];

   if(! media->toll) {
      ddsError("fatal error encountered on initial mount\n\tbinary= %s\n",
         bin->name);
      return DDS_BAD;
   } else if(! media->fp) {
      /* mount media, starting from the first */
      bin->LineTrc = 1;
   } else if(*mode == 'r') {
      /* mount subsequent read media */
      media->final_trace = bin->LineTrc;

      if(! media->next) {
         /* all files on all media have already been consumed */
         ddsOperator("all tapes read from %s\n", media->name);
         return DDS_BAD;
      }

      if(ddsMediaUnload(media)) return DDS_BAD;

      media->next->fp = media->fp;
      media->fp = NULL;
      bin->media = media = media->next;
   } else {
      /* mount subsequent write media */
      media->final_trace = bin->LineTrc;

      if(! media->next) {
         /* create another media descriptor */
         n_label = 1;

         for(chain = media->prev; chain; chain = chain->prev) {
            n_label++;
         }

         sprintf(scratch, "(%i-scratch)", ++n_label);

         chain = (DDS_MEDIA*)ddsZalloc(sizeof(DDS_MEDIA));
         media->next = chain;
         chain->prev = media;

         chain->label = ddsString(scratch);
         ddsStrCpyCat4(&chain->title, dds_media_genus[media->genus],
            " ", media->name, scratch);

         ddsMediaChain(media, chain);
      }

      if(ddsMediaUnload(media)) return DDS_BAD;

      media->next->fp = media->fp;
      media->fp = NULL;
      bin->media = media = media->next;
   }

   /* load the next media, if it exists */
   if(ddsMediaLoad(media, mode)) return DDS_BAD;

   media->first_trace = bin->LineTrc - 1;

   bin->ReelTrc = 1;
   bin->FileTrc = 1;
   bin->state = (DDS_BIN_STATE)(DDS_BIN_EOF | (bin->state & ~ DDS_BIN_EOM));

   media->balance = media->length * INCHES_PER_FOOT * media->duty;

   return DDS_OK;
}
