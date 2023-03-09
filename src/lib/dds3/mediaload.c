static char rcsid[]="$Id: mediaload.c 39 2009-01-13 23:00:15Z user $";
/*
RCS-t=DDS, load media on device

*/

/**********************************************/
/***   Copyright (C) 1996                   ***/
/***   AMOCO Corporation, Tulsa, OK 74102   ***/
/***   Randy Selzler, xx May 1996           ***/
/**********************************************/

#include "dds_mtio.h"
#if !defined(_WIN32) && !defined(MINGW)
#include <sys/fcntl.h>
#endif

int ddsMediaLoad(
   DDS_MEDIA *media,
   const char *mode)
{
   static DDS_STRING fetch = NULL_STRING;
   DDS_MEDIA *chain;
   GFILE *fp;
#if !defined(_WIN32) && !defined(MINGW) && !defined(Darwin)
   struct mtget mtiocget;
#endif
   int mount_loop, mount_prompt;

   ddsStrSetMin(&fetch, "", 128);

   /* 'fetch' string is list of subsequent tapes, if any */
   for(chain = media->next; chain; chain = chain->next) {
      ddsStrCat2(&fetch, " ", chain->label);
   }

   mount_prompt = DDS_MOUNT_PROMPT;

   for(mount_loop = 0; DDS_TRUE; mount_loop++) {
      if(! (mount_loop % mount_prompt)) {
         /* write message to operator requesting tape mount */
         if(*fetch.ptr) {
            ddsOperator("mount %s %s\n\tprefetch=%s\n",
               ((*mode == 'r') ? "read" : "write"),
               media->title.ptr, fetch.ptr);
         } else {
            ddsOperator("mount %s %s\n",
               ((*mode == 'r') ? "read" : "write"),
               media->title.ptr);
         }

         /* increase time between operator messages */
         mount_prompt *= DDS_MOUNT_DAMPER;
      }

      if(media->fp) {
#if !defined(_WIN32) && !defined(MINGW)
         /* The device has already been opened. */
         if(! ddsMTLOAD(gfileno(media->fp))) {
            /* load successful */
            break;
         }
#endif
         if(errno != EIO && errno != EAGAIN) {
            ddsError("%s\n\tloading %s %s\n",
               strerror(errno), ((*mode == 'r') ? "read" : "write"),
               media->title.ptr);
            return DDS_BAD;
         }
      } else {
         /* open file for device (media drive) */
         media->fp = gopen(media->name, mode);

#ifdef HOST_SGI
         if(media->fp) {
            ioctl(gfileno(media->fp), MTIOCGET, (char*)&mtiocget);

            if(mtiocget.mt_dsreg & MT_ONL) break;

            gclose(media->fp);
            media->fp = NULL;
            errno = EAGAIN;
         }
#else /* HOST_SGI */
         if(media->fp)  break;
#endif /* HOST_SGI */

         #ifdef HOST_SGI
            if(errno != EAGAIN &&     /* No such device or address */
               errno != EBUSY)        /* Mount device busy */
         #else
            if(errno != EIO &&        /* I/O error */
               errno != EBUSY)        /* Mount device busy */
         #endif 
         {
               ddsError("%s\n\tmount %s, %s\n",
                  strerror(errno),
                  media->title.ptr, (*mode == 'r') ? "read" : "write");
               return DDS_BAD;
         }
      }

      /* sleep while waiting for operator to mount tape */
#if defined(_WIN32) || defined(MINGW)
      Sleep((int)DDS_MOUNT_SLEEP*1000);
#else
      sleep((unsigned)DDS_MOUNT_SLEEP); 
#endif
   }

   media->file_count = 1;

   if(*mode == 'r' && media->file_skip) {
      /* skip files (internal labels) on each media */
#if !defined(_WIN32) && !defined(MINGW)
      ddsMTFSF(gfileno(media->fp), media->file_skip);
#endif
      media->file_count += media->file_skip;
   }

   return DDS_OK;
}
