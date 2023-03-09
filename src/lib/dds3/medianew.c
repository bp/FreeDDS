static char rcsid[]="$Id: medianew.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, malloc and default DDS_MEDIA (media descriptor).

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_MEDIA * ddsMediaNew(
   DDS_DICT *dict,
   DDS_BIN *bin,
   DDS_MEDIA_STATE state,
   const char *name,
   GFILE *fp)
{
   char *str;
   DDS_MEDIA *media;

   media = (DDS_MEDIA*)ddsZalloc(sizeof(DDS_MEDIA));

   media->state = state;

   media->fp = fp;

   if(fp) {
      media->genus = ddsFstatGenus(gfileno(fp));
   } else if(state & DDS_MEDIA_BUFFER) {
      media->genus = DDS_MEDIA_NULL;
   } else if(name) {
      media->genus = ddsStatGenus(name);
   } else {
      media->genus = DDS_MEDIA_NOT;
   }

   if(media->genus == DDS_MEDIA_ERR) {
      ddsError("media genus, %s\n\tname= %s\n",
         strerror(errno), name ? name : dict->name);
   } else if(media->genus == DDS_MEDIA_CHR &&
      name && strcmp(name, "/dev/null")) {
      /* special processing for moutable media (tapes) */
      media->state |= (DDS_MEDIA_STATE)(DDS_MEDIA_SWAP);
   }


   if(name) {
      media->name = ddsString(name);
      str = strchr(media->name, ' ');
      if (str) {
         *str = '\0';
         media->name = (const char*)ddsRealloc((void*)media->name, 
                                               5+strlen(media->name));
         strcat((char*)media->name, " ...");
      }
   } else {
      media->name = ddsString("<anonymous>");
   }

   media->hash = ddsHash(media->name);

   ddsStrCpyCat3(&media->title, dds_media_genus[media->genus],
      " ", media->name);

   media->dict = dict;
   media->bin = bin;

   media->mount = DDS_MEDIA_FILE;
   media->density = -1;

   /* last, to avoid cleanup faults */
   media->left = NULL;
   media->right = dds_media_list;
   if(dds_media_list) dds_media_list->left = media;
   dds_media_list = media;

   return media;
}
