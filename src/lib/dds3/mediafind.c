static char rcsid[]="$Id: mediafind.c 16 2009-01-06 21:52:47Z user $";
/*
RCS-t=DDS, find media, given its name.

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

DDS_MEDIA * ddsMediaFind(
   const char *name,
   int fd)
{
   DDS_STAT_ID stat_id, stat_id_media;
   DDS_MEDIA *media;

   if(name && *name && fd >= 0) {
      /* compare both the name and file descriptor */
      ddsFStatID(&stat_id, fd);
      for(media = dds_media_list; media; media = media->right) {
         if(media->fp) {
            if((DDS_OK == ddsFStatID(&stat_id_media, gfileno(media->fp)) &&
               ! memcmp((void*)&stat_id.st_dev, (void*)&stat_id_media.st_dev, sizeof(stat_id.st_dev)) &&
               stat_id.st_ino == stat_id_media.st_ino) ||
               ! strcmp(name, media->name)) return media; /* found it */
         } else {
            if(! strcmp(name, media->name)) return media; /* found it */
         }
      }
   } else if(fd >= 0) {
      /* compare only the file descriptor */
      ddsFStatID(&stat_id, fd);
      for(media = dds_media_list; media; media = media->right) {
         if(media->fp) {
            if(DDS_OK == ddsFStatID(&stat_id_media, gfileno(media->fp)) &&
               ! memcmp((void*)&stat_id.st_dev, (void*)&stat_id_media.st_dev, sizeof(stat_id.st_dev)) &&
               stat_id.st_ino == stat_id_media.st_ino)
                  return media; /* found it */
         }
      }
   } else if(name && *name) {
      /* compare only the name */
      for(media = dds_media_list; media; media = media->right) {
         if(! strcmp(name, media->name)) return media; /* found it */
      }
   }

   return NULL; /* not found */
}
