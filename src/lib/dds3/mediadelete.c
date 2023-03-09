static char rcsid[]="$Id: mediadelete.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, close and free DDS_MEDIA (media descriptor).

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_sys.h"

int ddsMediaDelete(
   DDS_MEDIA *media)
{
   /* assumption: (media->dict == NULL && media->bin == NULL) */
   int status = DDS_OK;

#if defined(_WIN32) || defined(MINGW)
/*
   if (media->dict == NULL && media->bin == NULL) {
	   return DDS_BAD;
   }
*/
   assert(!media->dict && !media->bin);
#else
   assert(! media->dict && ! media->bin);
#endif

   if(media->fp) {
      if(gclose(media->fp)) {
         /* problem closing file pointer */
         if(errno != ENOENT) {
            /* ENOENT == Sun "No such file or directory" */
            /* Seems to occur when closing (GFILE*) for "stdin:" */
            /* Apparently the file descriptor was already closed. */
            dds_error = ddsWarn("%s\n\tclose %s\n",
               strerror(errno), media->title.ptr);
            status = DDS_BAD;
         }
      }
      media->fp = NULL;
   }

   /* unlink this media */
   if(media->right) media->right->left = media->left;
   if(media->left)
      media->left->right = media->right;
   else
      dds_media_list = media->right;

   if(media->next) media->next->prev = NULL;

   if(media->prev) media->prev->next = NULL;

   /* delete next siblings */
   if(media->next) {
      media->next->dict = NULL;
      media->next->bin  = NULL;
      media->next->prev = NULL;
      ddsMediaDelete(media->next);
   }

   /* delete previous siblings */
   if(media->prev) {
      media->prev->dict = NULL;
      media->prev->bin  = NULL;
      media->prev->prev = NULL;
      ddsMediaDelete(media->prev);
   }

   ddsFree((void*)media->name);
   ddsFree((void*)media->title.ptr);
   ddsFree((void*)media->label);
   ddsFree(media);

   return status;
}
