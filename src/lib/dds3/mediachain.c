static char rcsid[]="$Id: mediachain.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, chain another media

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

void ddsMediaChain(
   DDS_MEDIA *media,
   DDS_MEDIA *chain)
{
   chain->state = (DDS_MEDIA_STATE)(media->state | DDS_MEDIA_UNLOAD);

   chain->genus = media->genus;
   chain->name = ddsString(media->name);
   chain->hash = media->hash;
   chain->mount = media->mount;

   chain->dict = media->dict;
   chain->bin = media->bin;

   chain->file_skip = media->file_skip;

   chain->toll = media->toll;

   chain->rate = media->rate;
   chain->density = media->density;
   chain->duty = media->duty;
   chain->capacity = media->capacity;

   chain->sectors = media->sectors;
   chain->tracks = media->tracks;
   chain->cylinders = media->cylinders;

   chain->gap = media->gap;
   chain->passes = media->passes;
   chain->length = media->length;

   chain->min_block = media->min_block;
   chain->max_block = media->max_block;
   chain->opt_block = media->opt_block;

   /* last, to avoid cleanup problems if it aborts */
   chain->left = NULL;
   chain->right = dds_media_list;
   dds_media_list->left = chain;
   dds_media_list = chain;

   return;
}
